/*
 *   Copyright 2016 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.6
import QtQuick.Templates 2.0 as T
import org.kde.plasma.core 2.0 as PlasmaCore
import "private" as Private

T.Button {
    id: control

    implicitWidth: contentItem.implicitWidth + leftPadding + rightPadding
    implicitHeight: Math.max(units.gridUnit * 1.6, contentItem.implicitHeight + topPadding + bottomPadding)

    leftPadding: surfaceNormal.margins.left
    topPadding: surfaceNormal.margins.top
    rightPadding: surfaceNormal.margins.right
    bottomPadding: surfaceNormal.margins.bottom

    hoverEnabled: true //Qt.styleHints.useHoverEffects TODO: how to make this work in 5.7?

    contentItem: Label {
        text: control.text
        font: control.font
        opacity: enabled || control.highlighted || control.checked ? 1 : 0.4
        color: theme.buttonTextColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: PlasmaCore.FrameSvgItem {
        id: surfaceNormal
        anchors.fill: parent
        imagePath: "widgets/button"
        prefix: control.pressed ? "pressed" : "normal"
        Private.ButtonShadow {
            z: -1
            anchors.fill: parent
            state: {
                if (control.pressed) {
                    return "hidden"
                } else if (control.hovered) {
                    return "hover"
                } else if (control.activeFocus) {
                    return "focus"
                } else {
                    return "shadow"
                }
            }
        }
    }
}
