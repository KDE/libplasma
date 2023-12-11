/*
    SPDX-FileCopyrightText: 2020 Niccolò Venerandi <niccolo@venerandi.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Templates as T

import org.kde.plasma.core as PlasmaCore
import org.kde.ksvg as KSvg
import org.kde.plasma.plasmoid
import org.kde.kirigami as Kirigami

/**
 * Item to be used as a header or footer in plasmoids
 *
 * @inherit QtQuick.Templates.ToolBar
 */
T.ToolBar {
    id: control

    Layout.fillWidth: true
    bottomPadding: position === T.ToolBar.Footer ? 0 : -backgroundMetrics.getMargin("bottom")
    topPadding: position === T.ToolBar.Footer ? -backgroundMetrics.getMargin("top") : 0

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            contentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             contentHeight + topPadding + bottomPadding)

    leftInset: backgroundMetrics.getMargin("left")
    rightInset: backgroundMetrics.getMargin("right")
    topInset: position === T.ToolBar.Footer ? 0 : backgroundMetrics.getMargin("top")
    bottomInset: position === T.ToolBar.Footer ? backgroundMetrics.getMargin("bottom") : 0

    Kirigami.Theme.colorSet: position === T.ToolBar.Header ? Kirigami.Theme.Header : Kirigami.Theme.Window
    Kirigami.Theme.inherit: false

    background: KSvg.FrameSvgItem {
        id: headingSvg
        // This graphics has to back with the dialog background, so it can be used if:
        // * both this and the dialog background are from the current theme
        // * both this and the dialog background are from fallback
        visible: fromCurrentImageSet === backgroundSvg.fromCurrentImageSet
        imagePath: "widgets/plasmoidheading"
        prefix: control.position === T.ToolBar.Header ? "header" : "footer"
        KSvg.Svg {
            id: backgroundSvg
            imagePath: "dialogs/background"
        }

        enabledBorders: {
            let borders = KSvg.FrameSvg.LeftBorder | KSvg.FrameSvg.RightBorder;
            if (Plasmoid.position !== PlasmaCore.Types.TopEdge || position !== T.ToolBar.Header) {
                borders |= KSvg.FrameSvg.TopBorder;
            }
            if (Plasmoid.position !== PlasmaCore.Types.BottomEdge || position !== T.ToolBar.Footer) {
                borders |= KSvg.FrameSvg.BottomBorder;
            }
            return borders;
        }

        BackgroundMetrics {
            id: backgroundMetrics

            function getMargin(margin: string): real {
                const w = Window.window;

                // TODO: This shouldn't be duck-typed
                if (w && w.hasOwnProperty("leftPadding")
                      && w.hasOwnProperty("topPadding")
                      && w.hasOwnProperty("rightPadding")
                      && w.hasOwnProperty("bottomPadding")) {
                    switch (margin) {
                    case "left":
                        return -w.leftPadding;
                    case "top":
                        return -w.topPadding;
                    case "right":
                        return -w.rightPadding;
                    case "bottom":
                    default:
                        return -w.bottomPadding;
                    }
                } else if (!hasInset) {
                    return -headingSvg.fixedMargins[margin];
                } else {
                    return -fixedMargins[margin] + inset[margin];
                }
            }
        }
    }
}
