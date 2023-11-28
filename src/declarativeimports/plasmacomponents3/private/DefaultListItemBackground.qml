/*
    SPDX-FileCopyrightText: 2016 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

import QtQuick 2.1
//for Settings
import org.kde.ksvg 1.0 as KSvg
import org.kde.kirigami 2 as Kirigami

KSvg.FrameSvgItem {
    id: background

    imagePath: "widgets/listitem"
    prefix: control.highlighted || control.down ? "pressed" : "normal"

    visible: control.ListView.view ? control.ListView.view.highlight === null : true

    KSvg.FrameSvgItem {
        imagePath: "widgets/listitem"
        visible: !Kirigami.Settings.isMobile
        prefix: "hover"
        anchors.fill: parent
        opacity: control.hovered && !control.down ? 1 : 0
    }
}

