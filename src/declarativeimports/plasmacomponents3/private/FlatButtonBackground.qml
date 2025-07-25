/*
 * SPDX-FileCopyrightText: 2020 Noah Davis <noahadvs@gmail.com>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import org.kde.ksvg as KSvg
import org.kde.kirigami as Kirigami

Item {
    id: root
    required property bool hovered
    required property bool pressed
    required property bool checked
    required property bool focused

    property real leftMargin: surfaceHover.margins.left
    property real topMargin: surfaceHover.margins.top
    property real rightMargin: surfaceHover.margins.right
    property real bottomMargin: surfaceHover.margins.bottom
    property string usedPrefix: surfaceHover.usedPrefix

    ButtonShadow {
        anchors.fill: parent
        showShadow: !(root.checked || root.pressed) && root.usedPrefix === "normal"
    }

    ButtonFocus {
        anchors.fill: parent
        showFocus: root.focused && !root.pressed
        flat: true
    }

    // TODO: Maybe add a way to customize the look of normal state flat buttons with "toolbutton-normal"?
    // TODO: Maybe add a way to customize the background of focused flat buttons with "toolbutton-focus-background"?
    // TODO KF6: "flat" would be a more logical name than "toolbutton" since toolbuttons can be non-flat.
    KSvg.FrameSvgItem {
        id: surfaceHover
        anchors.fill: parent
        imagePath: "widgets/button"
        /* TODO KF6: making "toolbutton-hover" margins work like "hover"
         * and using "hover" as a fallback would make more sense.
         * If that is done, make ButtonHover handle flat button hover effects.
         */
        // The fallback is "normal" to match PC2 behavior. Some 3rd party themes depend on this.
        prefix: ["toolbutton-hover", "normal"]
        visible: root.hovered
    }

    KSvg.FrameSvgItem {
        id: surfacePressed
        anchors.fill: parent
        imagePath: "widgets/button"
        prefix: ["toolbutton-pressed", "pressed"]
        opacity: root.checked || root.pressed ? 1 : 0
        Behavior on opacity {
            enabled: Kirigami.Units.shortDuration > 0
            NumberAnimation { duration: Kirigami.Units.shortDuration; easing.type: Easing.OutQuad }
        }
    }
}
