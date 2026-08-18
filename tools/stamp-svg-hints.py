#!/usr/bin/env python3
"""Works out which frame parts of a Plasma theme can be drawn cheaply, and writes the hints which say so.

KSvg cannot know from an svg whether a centre is one flat colour, or whether a border only varies across
its thickness: it would have to rasterise the element to find out, once per frame variant, at runtime. The
answer never changes for a given theme, so it belongs in the theme. This asks the question once, here, and
writes the hints KSvg reads:

    hint-solid-color                     the centre is one flat colour, so it is drawn as that colour
    hint-stretch-center-horizontally     the centre's picture repeats along its width
    hint-stretch-center-vertically       the centre's picture repeats along its height
    hint-stretch-borders                 reported on, never written: it is a theme author's decision

Nothing is written without --write. Run it on a checkout and read the diff.

    stamp-svg-hints.py src/desktoptheme/breeze
    stamp-svg-hints.py src/desktoptheme/breeze --write
"""

import argparse
import gzip
import re
import sys
from pathlib import Path

from PySide6.QtCore import QSize, Qt
from PySide6.QtGui import QGuiApplication, QImage, QPainter
from PySide6.QtSvg import QSvgRenderer

# The size a question is asked at, whatever the element's own size is. At its own size a shape can lose
# what makes it not flat: a corner radius or a gradient finer than a pixel comes out as one colour there
# and shows itself once the frame stretches it.
FLOOR = 32

SIDES = ("top", "bottom", "left", "right", "topleft", "topright", "bottomleft", "bottomright", "center")
HINTS = ("hint-solid-color", "hint-stretch-center-horizontally", "hint-stretch-center-vertically")


def read(path):
    return gzip.open(path, "rb").read().decode() if path.suffix == ".svgz" else path.read_text()


def write(path, text):
    if path.suffix == ".svgz":
        path.write_bytes(gzip.compress(text.encode()))
    else:
        path.write_text(text)


def ids(text):
    return [m.group(1) for m in re.finditer(r'id="([^"]+)"', text)]


def frames(text):
    """The prefixes whose nine parts are all present, which is what a frame is assembled from."""
    seen = {}
    for one in dict.fromkeys(ids(text)):
        if one in SIDES:
            seen.setdefault("", set()).add(one)
            continue
        longest = max((s for s in SIDES if one.endswith("-" + s)), key=len, default=None)
        if longest:
            seen.setdefault(one[: -len(longest) - 1], set()).add(longest)
    return sorted(prefix for prefix, parts in seen.items() if set(SIDES) <= parts)


def render(renderer, element, size):
    image = QImage(size, QImage.Format_ARGB32)
    image.fill(Qt.transparent)
    painter = QPainter(image)
    renderer.render(painter, element)
    painter.end()
    return image


def describe(renderer, element):
    """What one rasterisation says: whether it is one colour, draws nothing, or repeats along an axis."""
    bounds = renderer.boundsOnElement(element)
    if bounds.isEmpty():
        return None
    size = QSize(max(int(bounds.width() + 0.5), FLOOR), max(int(bounds.height() + 0.5), FLOOR))
    image = render(renderer, element, size)
    if image.isNull():
        return None

    rows = [[image.pixel(x, y) for x in range(image.width())] for y in range(image.height())]
    along_x = all(all(p == row[0] for p in row) for row in rows)
    along_y = all(row == rows[0] for row in rows)
    flat = along_x and along_y
    blank = flat and (rows[0][0] >> 24) == 0
    return {"flat": flat and not blank, "blank": blank, "along_x": along_x, "along_y": along_y}


def hint_element(name, at):
    # The shape the themes already use for a hint: a small rect, never painted, only asked about.
    return f'  <rect id="{name}" x="{at}" y="0" width="5" height="5" style="fill:#ff6600" />\n'


def stamp(path, decisions, spare_x):
    text = read(path)
    additions = ""
    for prefix, hints in sorted(decisions.items()):
        for hint in hints:
            name = f"{prefix}-{hint}" if prefix else hint
            if f'id="{name}"' in text:
                continue
            additions += hint_element(name, spare_x)
            spare_x += 8
    if not additions:
        return False
    closing = text.rindex("</svg>")
    write(path, text[:closing] + additions + text[closing:])
    return True


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("theme", type=Path, help="a desktoptheme directory, or a single svg")
    parser.add_argument("--write", action="store_true", help="write the hints, rather than only reporting them")
    parser.add_argument("--quiet", action="store_true", help="only the totals")
    args = parser.parse_args()

    QGuiApplication(sys.argv)

    files = [args.theme] if args.theme.is_file() else sorted(list(args.theme.rglob("*.svg")) + list(args.theme.rglob("*.svgz")))
    if not files:
        raise SystemExit(f"no svgs under {args.theme}")

    counted = {"solid": 0, "blank": 0, "one axis": 0, "neither": 0, "frames": 0}
    stretch_borders_kept = []
    touched = 0

    for path in files:
        text = read(path)
        prefixes = frames(text)
        if not prefixes:
            continue
        renderer = QSvgRenderer(str(path))
        if not renderer.isValid():
            print(f"  {path}: not readable by QSvgRenderer", file=sys.stderr)
            continue

        # A hint is written past the artwork rather than over it.
        spare_x = int(renderer.viewBoxF().width() + 8)
        decisions = {}
        for prefix in prefixes:
            counted["frames"] += 1
            named = (lambda part, p=prefix: f"{p}-{part}" if p else part)
            answer = describe(renderer, named("center"))
            if answer is None:
                continue
            hints = []
            if answer["blank"]:
                counted["blank"] += 1
            elif answer["flat"]:
                counted["solid"] += 1
                hints.append("hint-solid-color")
            elif answer["along_x"] or answer["along_y"]:
                counted["one axis"] += 1
                if answer["along_x"]:
                    hints.append("hint-stretch-center-horizontally")
                if answer["along_y"]:
                    hints.append("hint-stretch-center-vertically")
            else:
                counted["neither"] += 1
            if hints:
                decisions[prefix] = hints

            # Whether the borders a theme asks to stretch can be stretched from their own size is worth
            # reporting: with the new drawing they are, and a border which varies along its length will
            # look different than it did.
            asks_stretch = f'id="hint-stretch-borders"' in text or f'id="{named("hint-stretch-borders")}"' in text
            if asks_stretch:
                for side, axis in (("top", "x"), ("bottom", "x"), ("left", "y"), ("right", "y")):
                    side_answer = describe(renderer, named(side))
                    if side_answer is None:
                        continue
                    repeats = side_answer["along_x"] if axis == "x" else side_answer["along_y"]
                    if not repeats:
                        stretch_borders_kept.append(f"{path.name}:{named(side)}")

        if decisions and not args.quiet:
            for prefix, hints in sorted(decisions.items()):
                print(f"  {path}: {prefix or '(no prefix)'} -> {', '.join(hints)}")
        if decisions and args.write and stamp(path, decisions, spare_x):
            touched += 1

    print(f"\n{counted['frames']} frames: {counted['solid']} one colour, {counted['blank']} draw nothing, "
          f"{counted['one axis']} repeat along one axis, {counted['neither']} neither")
    if stretch_borders_kept:
        print(f"{len(stretch_borders_kept)} borders of hint-stretch-borders frames vary along their length, so "
              f"stretching them from their own size will not look the same:")
        for one in stretch_borders_kept[:12]:
            print(f"  {one}")
        if len(stretch_borders_kept) > 12:
            print(f"  and {len(stretch_borders_kept) - 12} more")
    if args.write:
        print(f"{touched} files written")
    else:
        print("nothing written, pass --write to stamp the hints")


if __name__ == "__main__":
    main()
