/*
    SPDX-FileCopyrightText: 2018 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/
pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Templates as T
import org.kde.kirigami as Kirigami

/*!
     \qmltype PageIndicator
     \inqmlmodule org.kde.plasma.components
     \brief A simple page indicator whose visual styling is determined by the active Plasma theme.
 */
T.PageIndicator {
    id: control

    implicitWidth: contentItem.implicitWidth + leftPadding + rightPadding
    implicitHeight: contentItem.implicitHeight + topPadding + bottomPadding

    padding: Kirigami.Units.smallSpacing
    spacing: Kirigami.Units.smallSpacing

    delegate: Rectangle {
        required property int index

        implicitWidth: Kirigami.Units.largeSpacing
        implicitHeight: implicitWidth

        radius: width
        color: Kirigami.Theme.textColor

        opacity: index === control.currentIndex ? 0.9 : control.pressed ? 0.7 : 0.5
        Behavior on opacity {
            enabled: Kirigami.Units.longDuration > 0
            OpacityAnimator {
                duration: Kirigami.Units.longDuration
                easing.type: Easing.InOutQuad
            }
        }
    }

    contentItem: Row {
        spacing: control.spacing

        Repeater {
            model: control.count
            delegate: control.delegate
        }
    }
}
