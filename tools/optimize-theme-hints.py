#!/usr/bin/env python3
"""Rewrites the tiling hints of a Plasma theme where a cheaper drawing gives the same pixels.

It sets the two hints which choose between repeating a frame part and stretching it:

    hint-tile-center      the center is drawn from a texture of its own size, repeated. Without it the
                          center is rendered again at every size the frame takes. Added where the
                          center is one flat colour, or draws nothing, so that repeating it is the same
                          picture as stretching it.
    hint-stretch-borders  the borders are rendered again at every size the frame takes. Without it each
                          border is drawn from a texture of its own size, repeated along its length.
                          Removed where every border only varies across its thickness, so that
                          repeating it is the same picture as stretching it.

Each change is checked by drawing the frame part both ways, the way FrameSvgItem does, at several
lengths and device pixel ratios. A part where any pixel differs by more than --tolerance is left alone
and reported. Frames KSvg paints with QPainter rather than with scene graph nodes, which are masks and
frames with hint-compose-over-border or an overlay, are never changed.

Nothing is written without --write.

    optimize-theme-hints.py src/desktoptheme/breeze
    optimize-theme-hints.py src/desktoptheme/breeze --write
"""

import argparse
import gzip
import math
import re
import sys
from pathlib import Path

import numpy as np
from PySide6.QtCore import QRectF, QSize, Qt
from PySide6.QtGui import QGuiApplication, QImage, QPainter
from PySide6.QtSvg import QSvgRenderer

BORDERS = ("top", "bottom", "left", "right")
CORNERS = ("topleft", "topright", "bottomleft", "bottomright")
PARTS = BORDERS + CORNERS + ("center",)

TILE_CENTER = "hint-tile-center"
STRETCH_BORDERS = "hint-stretch-borders"

# The content sizes a center is checked at, and the lengths a border is checked at.
CENTER_SIZES = ((240, 120), (613, 227), (97, 41), (7, 5))
BORDER_LENGTHS = (613, 120, 37, 5)


def read(path):
    return gzip.open(path, "rb").read().decode() if path.suffix == ".svgz" else path.read_text()


def write(path, text):
    if path.suffix == ".svgz":
        path.write_bytes(gzip.compress(text.encode(), mtime=0))
    else:
        path.write_text(text)


def ids(text):
    return list(dict.fromkeys(m.group(1) for m in re.finditer(r'\bid="([^"]+)"', text)))


def frames(names):
    """Each prefix which has at least one frame part, with the parts it has.

    The prefix is returned without its trailing '-', so the unprefixed frame is ''.
    """
    found = {}
    for name in names:
        if name.startswith("hint-") or "-hint-" in name:
            continue
        if name in PARTS:
            found.setdefault("", set()).add(name)
            continue
        part = max((p for p in PARTS if name.endswith("-" + p)), key=len, default=None)
        if part:
            found.setdefault(name[: -len(part) - 1], set()).add(part)
    return found


def named(prefix, name):
    return f"{prefix}-{name}" if prefix else name


def to_array(image):
    image = image.convertToFormat(QImage.Format_ARGB32_Premultiplied)
    data = np.frombuffer(image.constBits(), dtype=np.uint8, count=image.sizeInBytes())
    return data.reshape(image.height(), image.bytesPerLine())[:, : image.width() * 4].reshape(
        image.height(), image.width(), 4).astype(np.int16)


def render(renderer, element, size):
    """The element drawn to fill an image of the given size, like FrameSvg::image() does."""
    image = QImage(size, QImage.Format_ARGB32_Premultiplied)
    image.fill(Qt.transparent)
    painter = QPainter(image)
    renderer.render(painter, element, QRectF(0, 0, size.width(), size.height()))
    painter.end()
    return image


def native_size(renderer, element, scale):
    # FrameItemNode takes elementSize(), which is already multiplied by the device pixel ratio, and rounds it.
    bounds = renderer.transformForElement(element).mapRect(renderer.boundsOnElement(element))
    return QSize(round(bounds.width() * scale), round(bounds.height() * scale))


def repeated(tile, width, height, along_x, along_y):
    """What the GPU draws for a repeated texture: repeat on a repeating axis, a linear stretch on the other."""
    out = tile
    if along_x:
        out = np.tile(out, (1, math.ceil(width / out.shape[1]), 1))[:, :width]
    else:
        out = stretched(out, width, out.shape[0])
    if along_y:
        out = np.tile(out, (math.ceil(height / out.shape[0]), 1, 1))[:height]
    else:
        out = stretched(out, out.shape[1], height)
    return out


def stretched(array, width, height):
    if array.shape[1] == width and array.shape[0] == height:
        return array
    image = QImage(array.astype(np.uint8).tobytes(), array.shape[1], array.shape[0], array.shape[1] * 4,
                   QImage.Format_ARGB32_Premultiplied)
    return to_array(image.scaled(width, height, Qt.IgnoreAspectRatio, Qt.SmoothTransformation))


def difference(renderer, element, sizes, scales, along_x, along_y):
    """The largest channel difference between drawing the element repeated and drawing it stretched.

    None when the repeated drawing cannot be made, which FrameItemNode handles by not tiling at all.
    """
    worst = 0
    for scale in scales:
        native = native_size(renderer, element, scale)
        if native.isEmpty():
            return None
        tile = to_array(render(renderer, element, native))
        for width, height in sizes:
            width, height = round(width * scale), round(height * scale)
            if not along_x:
                width = native.width()
            if not along_y:
                height = native.height()
            ours = repeated(tile, width, height, along_x, along_y)
            theirs = to_array(render(renderer, element, QSize(width, height)))
            worst = max(worst, int(np.abs(ours - theirs).max()))
    return worst


def has_hint(names, prefix, hint):
    """KSvg reads a hint under the frame's prefix, or without any prefix for every frame of the file."""
    return hint in names or named(prefix, hint) in names


def hint_tag(name, x):
    # The shape the themes already use for a hint: a small rect, never painted, only asked about.
    return f'  <rect id="{name}" x="{x}" y="0" width="5" height="5" style="fill:#ff6600" />\n'


def drop(text, name):
    pattern = re.compile(r'[ \t]*<([a-zA-Z:]+)\b[^<>]*\bid="' + re.escape(name) + r'"[^<>]*(/>|>\s*</\1>)[ \t]*\n?')
    return pattern.subn("", text, count=1)


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("theme", type=Path, help="a desktoptheme directory, or a single svg or svgz")
    parser.add_argument("--write", action="store_true", help="write the changes, rather than only reporting them")
    parser.add_argument("--tolerance", type=int, default=2,
                        help="the largest channel difference, out of 255, still counted as the same pixel "
                             "(default 2)")
    parser.add_argument("--scales", default="1,1.25,1.5,2",
                        help="the device pixel ratios to check at (default 1,1.25,1.5,2)")
    parser.add_argument("--verbose", action="store_true", help="also name every frame part left alone, and why")
    args = parser.parse_args()
    scales = [float(s) for s in args.scales.split(",")]

    QGuiApplication(sys.argv)

    if args.theme.is_file():
        files = [args.theme]
    else:
        files = sorted(list(args.theme.rglob("*.svg")) + list(args.theme.rglob("*.svgz")))
    if not files:
        raise SystemExit(f"no svgs under {args.theme}")

    added = removed = touched = 0
    kept = []

    for path in files:
        text = read(path)
        names = set(ids(text))
        found = frames(names)
        if not found:
            continue
        renderer = QSvgRenderer(str(path))
        if not renderer.isValid():
            print(f"{path}: not readable by QSvgRenderer", file=sys.stderr)
            continue

        add = []
        drop_names = []
        tile_ok = {p for p in found if has_hint(names, p, TILE_CENTER)}
        borders_ok = {}

        # FrameSvgItem draws a frame with its own nodes, which is what the check above models. A frame which
        # composes its center over the borders, or has an overlay, is painted with QPainter instead, and so is
        # every mask. There repeating and stretching round to pixels differently at a fractional scale, so those
        # frames are left as they are, and a bare hint which would reach them is not changed either.
        painted = set()
        for prefix in found:
            composes = (named(prefix, "hint-compose-over-border") in names
                        and "mask-" + named(prefix, "center") in names)
            overlay = named(prefix, "overlay") in names
            if prefix == "mask" or prefix.startswith("mask-") or composes or overlay:
                painted.add(prefix)

        for prefix, parts in sorted(found.items()):
            label = f"{path}:{prefix or '(no prefix)'}"
            if prefix in painted:
                if args.verbose:
                    kept.append(f"{label}: painted with QPainter")
                continue

            if "center" in parts and not has_hint(names, prefix, TILE_CENTER):
                worst = difference(renderer, named(prefix, "center"), CENTER_SIZES, scales, True, True)
                if worst is None:
                    kept.append(f"{label}: center has no size to repeat")
                elif worst <= args.tolerance:
                    tile_ok.add(prefix)
                elif args.verbose:
                    kept.append(f"{label}: center differs by {worst} when repeated")

            sides = [side for side in BORDERS if side in parts]
            if sides:
                differs = []
                for side in sides:
                    along_x = side in ("top", "bottom")
                    sizes = [(length, 0) if along_x else (0, length) for length in BORDER_LENGTHS]
                    worst = difference(renderer, named(prefix, side), sizes, scales, along_x, not along_x)
                    if worst is None or worst > args.tolerance:
                        differs.append(f"{side} ({'no size' if worst is None else worst})")
                borders_ok[prefix] = not differs
                if differs and has_hint(names, prefix, STRETCH_BORDERS) and args.verbose:
                    kept.append(f"{label}: borders differ when repeated: {', '.join(differs)}")

        # The bare hint-tile-center is the unprefixed frame's own hint, and every other frame of the file reads
        # it too, so it is only written when all of them repeat cleanly.
        centers = [p for p, parts in found.items() if "center" in parts]
        for prefix in sorted(tile_ok - {p for p in found if has_hint(names, p, TILE_CENTER)}):
            if prefix:
                add.append(named(prefix, TILE_CENTER))
            elif all(p in tile_ok for p in centers):
                add.append(TILE_CENTER)
            else:
                others = [p for p in centers if p not in tile_ok]
                kept.append(f"{path}: not adding bare hint-tile-center, it would also tile {', '.join(others)}")

        # A prefixed hint-stretch-borders goes where that frame's borders repeat cleanly. The bare one applies
        # to every frame of the file, so it only goes when all of them do.
        for prefix in sorted(found):
            if prefix and named(prefix, STRETCH_BORDERS) in names and borders_ok.get(prefix):
                if STRETCH_BORDERS in names:
                    kept.append(f"{path}:{prefix}: own hint-stretch-borders, but the bare one still applies")
                else:
                    drop_names.append(named(prefix, STRETCH_BORDERS))
        if STRETCH_BORDERS in names:
            stretched_frames = [p for p in found if p in borders_ok or p in painted]
            if all(borders_ok.get(p) for p in stretched_frames):
                drop_names.extend([STRETCH_BORDERS] + [named(p, STRETCH_BORDERS) for p in stretched_frames
                                                       if p and named(p, STRETCH_BORDERS) in names])
            else:
                others = [p or "(no prefix)" for p in stretched_frames if not borders_ok.get(p)]
                kept.append(f"{path}: keeping bare hint-stretch-borders, needed by {', '.join(others)}")

        if not add and not drop_names:
            continue

        for name in add:
            print(f"{path}: + {name}")
        for name in drop_names:
            print(f"{path}: - {name}")
        added += len(add)
        removed += len(drop_names)

        if args.write:
            for name in drop_names:
                text, count = drop(text, name)
                if not count:
                    print(f"{path}: could not remove {name}, edit it by hand", file=sys.stderr)
            x = int(renderer.viewBoxF().right() + 8)
            closing = text.rindex("</svg>")
            text = text[:closing] + "".join(hint_tag(name, x + 8 * i) for i, name in enumerate(add)) + text[closing:]
            write(path, text)
            touched += 1

    if kept:
        print("\nleft alone:")
        for line in kept:
            print(f"  {line}")
    print(f"\n{added} hint-tile-center added, {removed} hint-stretch-borders removed")
    print(f"{touched} files written" if args.write else "nothing written, pass --write to apply")


if __name__ == "__main__":
    main()
