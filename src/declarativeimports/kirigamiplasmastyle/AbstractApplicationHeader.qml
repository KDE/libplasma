/*
    SPDX-FileCopyrightText: 2016 Marco Martin <mart@kde.org>
    SPDY-FileCopyrightText: 2020 Jonah Brüchert <jbb@kaidan.im>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

import QtQuick
import org.kde.kirigami as Kirigami

import "../../templates" as T
import "../../private" as KirigamiPrivate

T.AbstractApplicationHeader {
    id: root

    Kirigami.Theme.inherit: false
    Kirigami.Theme.colorSet: Kirigami.Theme.Header

    background: Rectangle {
        color: Kirigami.Theme.backgroundColor
        KirigamiPrivate.EdgeShadow {
            radius: Kirigami.Units.gridUnit * 0.5
            visible: root.separatorVisible
            anchors {
                right: parent.right
                left: parent.left
                top: parent.bottom
            }
            edge: Qt.TopEdge
        }
    }
}
