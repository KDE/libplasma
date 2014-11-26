/*
 *   Copyright 2014 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  2.010-1301, USA.
 */

import QtQuick 2.2
import QtQuick.Controls.Styles 1.2 as QtQuickControlStyle
import QtQuick.Controls 1.2
import org.kde.plasma.core 2.0 as PlasmaCore

QtQuickControlStyle.SwitchStyle {
    id: styleRoot

    handle: PlasmaCore.FrameSvgItem {
        opacity: control.enabled ? 1.0 : 0.6
        width: height
        imagePath: "widgets/button"
        prefix: "shadow"

        PlasmaCore.FrameSvgItem {
            id: button
            imagePath: "widgets/button"
            prefix: "normal"
            anchors.fill: parent
        }
    }

    groove: PlasmaCore.FrameSvgItem {
        imagePath: "widgets/slider"
        prefix: "groove"
        implicitHeight: theme.mSize(theme.defaultFont).height
        implicitWidth: height * 2

        PlasmaCore.FrameSvgItem {
            id: highlight
            imagePath: "widgets/slider"
            prefix: "groove-highlight"
            anchors.fill: parent

            opacity: checked ? 1 : 0
            Behavior on opacity {
                PropertyAnimation { duration: units.shortDuration * 2 }
            }
        }
    }
}
