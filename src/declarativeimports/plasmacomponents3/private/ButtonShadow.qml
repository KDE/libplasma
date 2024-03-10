/*
    SPDX-FileCopyrightText: 2011 Daker Fernandes Pinheiro <dakerfp@gmail.com>
    SPDX-FileCopyrightText: 2011 Marco Martin <mart@kde.org>
    SPDX-FileCopyrightText: 2020 Noah Davis <noahadvs@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

import QtQuick
import org.kde.ksvg as KSvg
import org.kde.kirigami as Kirigami

KSvg.FrameSvgItem {
    id: shadowEffect

    property bool showShadow

    anchors {
        fill: parent
        leftMargin: -margins.left
        topMargin: -margins.top
        rightMargin: -margins.right
        bottomMargin: -margins.bottom
    }
    imagePath: "widgets/button"
    prefix: "shadow"

    opacity: showShadow ? 1 : 0
    Behavior on opacity {
        enabled: Kirigami.Units.shortDuration > 0
        OpacityAnimator { duration: Kirigami.Units.shortDuration; easing.type: Easing.OutQuad }
    }
}
