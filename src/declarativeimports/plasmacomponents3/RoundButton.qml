/*
    SPDX-FileCopyrightText: 2018 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Templates as T
import org.kde.ksvg as KSvg
//NOTE: importing PlasmaCore is necessary in order to make KSvg load the current Plasma Theme
import org.kde.plasma.core as PlasmaCore
import org.kde.kirigami as Kirigami
import "private" as Private

T.RoundButton {
    id: control

    Accessible.role: Accessible.Button

    implicitWidth: Math.max(Kirigami.Units.gridUnit, contentItem.implicitWidth)
                            + leftPadding + rightPadding
    implicitHeight: Math.max(Kirigami.Units.gridUnit, contentItem.implicitHeight)
                            + topPadding + bottomPadding

    leftPadding: text.length > 0 ? surfaceNormal.margins.left : contentItem.extraSpace
    topPadding: text.length > 0 ? surfaceNormal.margins.top : contentItem.extraSpace
    rightPadding: text.length > 0 ? surfaceNormal.margins.right : contentItem.extraSpace
    bottomPadding: text.length > 0 ? surfaceNormal.margins.bottom : contentItem.extraSpace

    hoverEnabled: !Kirigami.Settings.tabletMode

    Kirigami.Theme.colorSet: Kirigami.Theme.Button
    Kirigami.Theme.inherit: false

    contentItem: RowLayout {
        // This is the spacing which will make the icon a square inscribed in the circle with an extra smallspacing of margins
        readonly property int extraSpace: implicitWidth / 2 - implicitWidth / 2 * Math.sqrt(2) / 2 + Kirigami.Units.smallSpacing
        Kirigami.Icon {
            Layout.preferredWidth: Kirigami.Units.iconSizes.smallMedium
            Layout.preferredHeight: Kirigami.Units.iconSizes.smallMedium
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: source.length > 0
            source: control.icon ? (control.icon.name || control.icon.source) : ""
        }
        Label {
            visible: text.length > 0
            text: control.text
            font: control.font
            opacity: enabled || control.highlighted || control.checked ? 1 : 0.75
            color: Kirigami.Theme.textColor
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
    }

    background: Item {
        id: backgroundItem

        opacity: control.enabled ? 1 : 0.6

        // Round Button

        KSvg.Svg {
            id: buttonSvg
            imagePath: "widgets/actionbutton"
            colorSet: KSvg.Svg.Button
        }

        Private.RoundShadow {
            id: roundShadow
            visible: !control.flat || control.activeFocus || control.highlighted
            anchors.fill: parent
            state: {
                if (control.down) {
                    return "hidden"
                } else if (control.enabled && control.hovered) {
                    return "hover"
                } else if (control.activeFocus || control.highlighted) {
                    return "focus"
                } else {
                    return "shadow"
                }
            }
        }

        KSvg.SvgItem {
            id: buttonItem
            svg: buttonSvg
            elementId: (control.down || control.checked) ? "pressed" : "normal"
            anchors.fill: parent
            //internal: if there is no hover status, don't paint on mouse over in touchscreens
            opacity: (control.down || control.checked || !control.flat || (roundShadow.hasOverState && control.enabled && control.hovered)) ? 1 : 0
            Behavior on opacity {
                enabled: Kirigami.Units.longDuration > 0
                PropertyAnimation { duration: Kirigami.Units.longDuration }
            }
        }

        // Normal Button
        // TODO: Make round button always round?

        readonly property bool useNormalButton: control.text.length > 0

        Private.ButtonShadow {
            anchors.fill: parent
            showShadow: backgroundItem.useNormalButton && !control.flat && (!control.down || !control.checked)
        }

        KSvg.FrameSvgItem {
            id: surfaceNormal
            anchors.fill: parent
            imagePath: "widgets/button"
            prefix: "normal"
            opacity: backgroundItem.useNormalButton && (!control.flat || (control.enabled && control.hovered) ) && (!control.down || !control.checked) ? 1 : 0
            Behavior on opacity {
                enabled: Kirigami.Units.longDuration > 0
                OpacityAnimator {
                    duration: Kirigami.Units.longDuration
                    easing.type: Easing.InOutQuad
                }
            }
        }

        Private.ButtonFocus {
            anchors.fill: parent
            showFocus: backgroundItem.useNormalButton && control.activeFocus && !control.down
        }

        Private.ButtonHover {
            anchors.fill: parent
            showHover: backgroundItem.useNormalButton && control.enabled && control.hovered && !control.down
        }

        KSvg.FrameSvgItem {
            anchors.fill: parent
            imagePath: "widgets/button"
            prefix: "pressed"
            visible: backgroundItem.useNormalButton
            opacity: control.checked || control.down ? 1 : 0
            Behavior on opacity {
                enabled: Kirigami.Units.longDuration > 0
                OpacityAnimator {
                    duration: Kirigami.Units.longDuration
                    easing.type: Easing.InOutQuad
                }
            }
        }
    }
}
