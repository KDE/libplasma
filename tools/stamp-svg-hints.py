#!/usr/bin/env python3
"""Works out which frame parts of a Plasma theme can be drawn cheaply, and writes the hints which say so.

KSvg cannot know from an svg whether a centre is one flat colour, or whether a border only varies across
its thickness: it would have to rasterise the element to find out, once per frame variant, at runtime. The
answer never changes for a given theme, so it belongs in the theme. This asks the question once, here, and
writes the hints KSvg reads:

    hint-solid-color                     the centre is one flat colour, so it is drawn as that colour
    hint-stretch-center-horizontally     the centre's picture repeats along its width
    hint-stretch-center-vertically       the centre's picture repeats along its height
    hint-uniform-borders                 every border varies only across its thickness, so a stretched
                                         border is drawn from its own size rather than rendered at every
                                         size the frame takes

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
from PySide6.QtGui import QGuiApplication, QImage, QPainter, QPixmap
from PySide6.QtSvg import QSvgRenderer

# The size a question is asked at, whatever the element's own size is. At its own size a shape can lose
# what makes it not flat: a corner radius or a gradient finer than a pixel comes out as one colour there
# and shows itself once the frame stretches it.
FLOOR = 32

SIDES = ("top", "bottom", "left", "right", "topleft", "topright", "bottomleft", "bottomright", "center")
HINTS = ("hint-solid-color", "hint-stretch-center-horizontally", "hint-stretch-center-vertically")

# The older hints this can also act on:
#
#   hint-tile-center     tiling one flat colour is filling with it, and a tiled texture is never atlased,
#                        by construction, since repeat wrapping needs a texture of its own. So where the
#                        centre is one colour the tile hint is removed and hint-solid-color put in its
#                        place: same pixels, no texture at all.
#   hint-stretch-borders added where every border of the frame only varies across its thickness. Tiling
#                        such a border and stretching it are the same picture, and the stretched one is
#                        atlasable where the tiled one is not.
#
# What it will not do is remove hint-stretch-borders from a frame whose borders vary along their length.
# That is a design decision: the hint asks for stretching, and with the new drawing those borders are
# stretched from their own size, which will not look as they do today.


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


def tiling_matches_stretching(renderer, element, axis, length=512):
    """Whether repeating the element along an axis and scaling it there give the same picture.

    A border which only varies across its thickness says the two should agree, and for most elements they
    do. Where the strip ends on a partly transparent pixel they do not: the repeat puts two such edges
    against each other and leaves a seam, which scaling never draws. That is a difference a reader sees, so
    it is measured here rather than assumed either way.
    """
    bounds = renderer.boundsOnElement(element)
    if bounds.isEmpty():
        return False
    native = QSize(max(int(bounds.width() + 0.5), 1), max(int(bounds.height() + 0.5), 1))
    strip = render(renderer, element, native)
    if strip.isNull():
        return False

    if axis == "x":
        tiled = QImage(QSize(length, native.height()), QImage.Format_ARGB32)
        stretched_size = QSize(length, native.height())
    else:
        tiled = QImage(QSize(native.width(), length), QImage.Format_ARGB32)
        stretched_size = QSize(native.width(), length)

    tiled.fill(Qt.transparent)
    painter = QPainter(tiled)
    painter.drawTiledPixmap(tiled.rect(), QPixmap.fromImage(strip))
    painter.end()

    stretched = strip.scaled(stretched_size, Qt.IgnoreAspectRatio, Qt.SmoothTransformation).convertToFormat(QImage.Format_ARGB32)
    tiled = tiled.convertToFormat(QImage.Format_ARGB32)

    for y in range(tiled.height()):
        for x in range(tiled.width()):
            a, b = tiled.pixel(x, y), stretched.pixel(x, y)
            if a != b:
                return False
    return True


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


def drop_element(text, name):
    """Removes a hint element, which the themes write as one self-closing tag."""
    pattern = re.compile(r"[ \t]*<[a-zA-Z]+[^<>]*id=\"" + re.escape(name) + r"\"[^<>]*/>\n?", re.S)
    new_text, count = pattern.subn("", text)
    return (new_text, True) if count else (text, False)


def hint_element(name, at):
    # The shape the themes already use for a hint: a small rect, never painted, only asked about.
    return f'  <rect id="{name}" x="{at}" y="0" width="5" height="5" style="fill:#ff6600" />\n'


def bare_is_safe(decisions, prefixes, hint):
    """A hint without a prefix applies to every frame in the file, which is how KSvg reads it.

    So it can only be written when every frame of the file wants it. Otherwise the unprefixed frame goes
    unhinted rather than dragging its neighbours onto the wrong path.
    """
    return all(hint in decisions.get(prefix, ()) for prefix in prefixes)


def stamp(path, decisions, spare_x, removals=None):
    text = read(path)
    changed = False
    for prefix, gone in (removals or {}).items():
        for hint in gone:
            name = f"{prefix}-{hint}" if prefix else hint
            text, dropped = drop_element(text, name)
            changed = changed or dropped
    additions = ""
    for prefix, hints in sorted(decisions.items()):
        for hint in hints:
            name = f"{prefix}-{hint}" if prefix else hint
            if f'id="{name}"' in text:
                continue
            additions += hint_element(name, spare_x)
            spare_x += 8
    if not additions and not changed:
        return False
    closing = text.rindex("</svg>")
    write(path, text[:closing] + additions + text[closing:])
    return True


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("theme", type=Path, help="a desktoptheme directory, or a single svg")
    parser.add_argument("--write", action="store_true", help="write the hints, rather than only reporting them")
    parser.add_argument("--quiet", action="store_true", help="only the totals")
    parser.add_argument("--keep-tiled-borders", action="store_true",
                        help="leave a tiled frame's borders tiled, whatever their artwork allows. Only frames "
                             "which already ask for stretched borders are hinted, which is how a theme reads "
                             "before any border is converted")
    parser.add_argument("--add-stretch-borders", action="store_true",
                        help="add hint-stretch-borders to tiled frames whose borders only vary across their "
                             "thickness even where a tile seam makes the two draw differently. Frames where "
                             "they draw the same are converted without this")
    args = parser.parse_args()

    QGuiApplication(sys.argv)

    files = [args.theme] if args.theme.is_file() else sorted(list(args.theme.rglob("*.svg")) + list(args.theme.rglob("*.svgz")))
    if not files:
        raise SystemExit(f"no svgs under {args.theme}")

    counted = {"solid": 0, "blank": 0, "one axis": 0, "neither": 0, "frames": 0,
               "borders stretchable": 0, "tiled centres replaced": 0, "uniform borders": 0,
               "borders converted": 0}
    stretch_borders_kept = []
    seams_kept = []
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
        removals = {}
        for prefix in prefixes:
            counted["frames"] += 1
            named = (lambda part, p=prefix: f"{p}-{part}" if p else part)
            answer = describe(renderer, named("center"))
            if answer is None:
                continue
            hints = []
            if answer["blank"]:
                # An empty center is one colour as well, a transparent one, and KSvg draws nothing at all
                # for it. Worth the hint: it is what a shadow frame's center is, and without it the empty
                # element is rendered and uploaded at every size the frame takes.
                counted["blank"] += 1
                hints.append("hint-solid-color")
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

            # The borders: whether each only varies across its thickness decides both whether the stretch
            # hint can be added and whether an existing one now draws differently.
            sides = {}
            for side, axis in (("top", "x"), ("bottom", "x"), ("left", "y"), ("right", "y")):
                side_answer = describe(renderer, named(side))
                if side_answer is None:
                    continue
                sides[side] = side_answer["along_x"] if axis == "x" else side_answer["along_y"]

            asks_stretch = 'id="hint-stretch-borders"' in text or f'id="{named("hint-stretch-borders")}"' in text
            if asks_stretch:
                if sides and all(sides.values()):
                    # Stretched and uniform across their thickness: the picture at any length is what the
                    # GPU makes from the element's own texture, so the re-render at every size can go.
                    counted["uniform borders"] += 1
                    decisions.setdefault(prefix, []).append("hint-uniform-borders")
                else:
                    for side, repeats in sides.items():
                        if not repeats:
                            stretch_borders_kept.append(f"{path.name}:{named(side)}")
            elif args.keep_tiled_borders:
                # Asked to leave tiling alone, so a tiled frame is not looked at further.
                pass
            elif sides and all(sides.values()):
                # Every border here only varies across its thickness, so it can be drawn from the element's
                # own size. The frame tiles them today, and tiling repeats a strip where stretching scales
                # it, so the question is whether the two draw the same picture. Where they do, saying so
                # costs the theme nothing and is written; where a seam shows, the frame is named instead.
                same = all(tiling_matches_stretching(renderer, named(side), axis)
                           for side, axis in (("top", "x"), ("bottom", "x"), ("left", "y"), ("right", "y"))
                           if side in sides)
                counted["borders stretchable"] += 1
                if same:
                    counted["borders converted"] += 1
                    decisions.setdefault(prefix, []).append("hint-stretch-borders")
                    decisions.setdefault(prefix, []).append("hint-uniform-borders")
                elif args.add_stretch_borders:
                    counted["borders converted"] += 1
                    decisions.setdefault(prefix, []).append("hint-stretch-borders")
                    decisions.setdefault(prefix, []).append("hint-uniform-borders")
                else:
                    seams_kept.append(f"{path.name}:{prefix or '(no prefix)'}")

            # A tiled centre which is one colour is a texture, and a standalone one at that, for a fill.
            tiles = 'id="hint-tile-center"' in text or f'id="{named("hint-tile-center")}"' in text
            if tiles and (answer["flat"] or answer["blank"]):
                removals.setdefault(prefix, []).append("hint-tile-center")

        # A hint with no prefix is read as applying to every frame of the file, so it is only written when
        # they all want it. widgets/pager.svgz is the case in point: its unprefixed centre is one colour
        # while its normal centre is a gradient, and the bare hint would have flattened the gradient.
        if "" in decisions:
            kept = [h for h in decisions[""] if bare_is_safe(decisions, prefixes, h)]
            dropped = [h for h in decisions[""] if h not in kept]
            if dropped and not args.quiet:
                print(f"  {path}: not writing {', '.join(dropped)} without a prefix, other frames here disagree")
            if kept:
                decisions[""] = kept
            else:
                del decisions[""]

        # Dropping a bare element takes the hint from every frame of the file too, so a tiled centre
        # which is one colour in one frame cannot drop it while another frame still needs it. The frames
        # which asked only because of the bare element have nothing of their own to drop, so they go too.
        if "" in removals:
            for hint in list(removals[""]):
                if bare_is_safe(removals, prefixes, hint):
                    continue
                removals[""].remove(hint)
                for prefix in prefixes:
                    if prefix and hint in removals.get(prefix, ()) and f'id="{prefix}-{hint}"' not in text:
                        removals[prefix].remove(hint)
                if not args.quiet:
                    print(f"  {path}: keeping {hint}, other frames here still need it")
            removals = {prefix: hints for prefix, hints in removals.items() if hints}

        counted["tiled centres replaced"] += sum(len(hints) for hints in removals.values())

        if decisions and not args.quiet:
            for prefix, hints in sorted(decisions.items()):
                print(f"  {path}: {prefix or '(no prefix)'} -> {', '.join(hints)}")
        if args.write and (decisions or removals) and stamp(path, decisions, spare_x, removals):
            touched += 1
        if removals and not args.quiet:
            for prefix, gone in sorted(removals.items()):
                print(f"  {path}: {prefix or '(no prefix)'} -> dropping {', '.join(gone)}, the centre is one colour")

    print(f"\n{counted['frames']} frames: {counted['solid']} one colour, {counted['blank']} draw nothing, "
          f"{counted['one axis']} repeat along one axis, {counted['neither']} neither")
    print(f"borders: {counted['uniform borders']} stretched frames whose borders vary only across their thickness, "
          f"so drawn from their own size")
    print(f"older hints: {counted['tiled centres replaced']} frames whose tiled centre becomes a colour, "
          f"{counted['borders stretchable']} tiled frames whose borders could be stretched instead, "
          f"{counted['borders converted']} of them converted")
    if stretch_borders_kept:
        print(f"{len(stretch_borders_kept)} borders of hint-stretch-borders frames vary along their length, so "
              f"stretching them from their own size will not look the same:")
        for one in stretch_borders_kept[:12]:
            print(f"  {one}")
        if len(stretch_borders_kept) > 12:
            print(f"  and {len(stretch_borders_kept) - 12} more")
    if seams_kept:
        print(f"{len(seams_kept)} tiled frames whose borders repeat but tile with a visible seam, so drawing "
              f"them stretched is cheaper and not the same picture. Pass --add-stretch-borders to take them:")
        for one in seams_kept[:12]:
            print(f"  {one}")
        if len(seams_kept) > 12:
            print(f"  and {len(seams_kept) - 12} more")
    if args.write:
        print(f"{touched} files written")
    else:
        print("nothing written, pass --write to stamp the hints")


if __name__ == "__main__":
    main()
