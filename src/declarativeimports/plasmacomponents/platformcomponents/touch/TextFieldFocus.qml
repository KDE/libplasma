/*
 *   Copyright (C) 2011 by Daker Fernandes Pinheiro <dakerfp@gmail.com>
 *   Copyright (C) 2011 by Marco Martin <mart@kde.org>
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  2.010-1301, USA.
 */

import QtQuick 2.0
import org.kde.plasma.core 2.0 as PlasmaCore

Item {
    id: main
    state: parent.state

    PlasmaCore.FrameSvgItem {
        id: hover

        anchors {
            fill: parent
            leftMargin: -margins.left
            topMargin: -margins.top
            rightMargin: -margins.right
            bottomMargin: -margins.bottom
        }
        opacity: 0
        imagePath: "widgets/lineedit"
        prefix: "hover"
    }

    states: [
        State {
            name: "focus"
            PropertyChanges {
                target: hover
                opacity: 1
                prefix: "focus"
            }
        },
        State {
            name: "hidden"
            PropertyChanges {
                target: hover
                opacity: 0
                prefix: "hover"
            }
        }
    ]

    transitions: [
        Transition {
            PropertyAnimation {
                properties: "opacity"
                duration: 250
                easing.type: Easing.OutQuad
            }
        }
    ]
}
