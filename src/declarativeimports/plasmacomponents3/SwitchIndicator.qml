/*
    SPDX-FileCopyrightText: 2016 Marco Martin <mart@kde.org>
    SPDX-FileCopyrightText: 2022 ivan (@ratijas) tkachenko <me@ratijas.tk>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

import QtQuick
import QtQuick.Templates as T
import org.kde.ksvg as KSvg
import org.kde.kirigami as Kirigami
import "private" as Private

Item {
    id: root

    required property T.AbstractButton control

    implicitWidth: inactive.implicitWidth
    implicitHeight: Math.max(inactive.implicitHeight, button.implicitHeight)

    layer.enabled: opacity < 1
    opacity: control.enabled ? 1 : 0.6

    KSvg.Svg {
        id: switchSvg
        imagePath: "widgets/switch"
        // FIXME
        colorSet: control.Kirigami.Theme.colorSet
    }

    KSvg.FrameSvgItem {
        id: inactive
        anchors {
            left: parent.left
            right: parent.right
            leftMargin: 1
            rightMargin: 1
            verticalCenter: parent.verticalCenter
        }
        implicitHeight: switchSvg.hasElement("hint-bar-size")
                ? switchSvg.elementSize("hint-bar-size").height
                : button.implicitHeight
        implicitWidth: switchSvg.hasElement("hint-bar-size")
                ? switchSvg.elementSize("hint-bar-size").width
                : root.implicitHeight * 2
        imagePath: "widgets/switch"
        prefix: "inactive"
    }
    KSvg.FrameSvgItem {
        anchors {
            left: inactive.left
            top: inactive.top
            bottom: inactive.bottom
            right: button.right
        }
        imagePath: "widgets/switch"
        prefix: "active"
    }
    KSvg.SvgItem {
        id: button

        x: Math.max(0, Math.min(parent.width - width, control.visualPosition * parent.width - (width / 2)))

        anchors.verticalCenter: parent.verticalCenter

        svg: switchSvg
        elementId: control.down ? "handle-pressed" : (control.hovered || control.focus ? "handle-hover" : "handle")

        implicitWidth: naturalSize.width
        implicitHeight: naturalSize.height

        Behavior on x {
            enabled: !root.control.down
            // Can't use XAnimator, since it doesn't update x during the animation, so the active
            // background is not animated.
            NumberAnimation {
                duration: Kirigami.Units.shortDuration
                easing.type: Easing.InOutQuad
            }
        }
        KSvg.SvgItem {
            svg: switchSvg
            z: -1
            anchors.centerIn: parent
            implicitWidth: naturalSize.width
            implicitHeight: naturalSize.height
            elementId: "handle-shadow"
            visible: enabled && !control.down
        }
        KSvg.SvgItem {
            anchors.centerIn: parent
            implicitWidth: naturalSize.width
            implicitHeight: naturalSize.height
            svg: switchSvg
            elementId: "handle-focus"
            visible: opacity > 0
            opacity: control.visualFocus
            Behavior on opacity {
                NumberAnimation {
                    duration: Kirigami.Units.longDuration
                    easing.type: Easing.OutCubic
                }
            }
        }
    }
}

