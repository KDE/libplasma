/*
 * SPDX-FileCopyrightText: 2017 Marco Martin <notmart@gmail.com>
 * SPDX-FileCopyrightText: 2020 Nate Graham <nate@kde.org>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Templates as T
import org.kde.ksvg as KSvg
//NOTE: importing PlasmaCore is necessary in order to make KSvg load the current Plasma Theme
import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.components as PlasmaComponents3
import org.kde.kirigami as Kirigami
import "private" as P

T.SpinBox {
    id: control

    implicitWidth: Math.max(
        implicitBackgroundWidth + leftInset + rightInset,
        Math.max(implicitContentWidth, Kirigami.Units.gridUnit)
            + spacing * 2 + leftPadding + rightPadding,
        up.implicitIndicatorWidth + down.implicitIndicatorWidth
    )
    implicitHeight: Math.max(
        implicitBackgroundHeight + topInset + bottomInset,
        implicitContentHeight + topPadding + bottomPadding,
        up.implicitIndicatorHeight,
        down.implicitIndicatorHeight
    )

    leftPadding: !mirrored ? down.implicitIndicatorWidth : up.implicitIndicatorWidth
    rightPadding: mirrored ? down.implicitIndicatorWidth : up.implicitIndicatorWidth
    topPadding: bgLoader.topMargin
    bottomPadding: bgLoader.bottomMargin
    spacing: bgLoader.leftMargin
    editable: true
    inputMethodHints: Qt.ImhFormattedNumbersOnly
    validator: IntValidator {
        locale: control.locale.name
        bottom: Math.min(control.from, control.to)
        top: Math.max(control.from, control.to)
    }
    wheelEnabled: true
    hoverEnabled: Qt.styleHints.useHoverEffects

    KSvg.Svg {
        id: lineSvg
        imagePath: "widgets/line"
    }

    up.indicator: P.FlatButtonBackground {
        x: control.mirrored ? 0 : parent.width - width
        implicitHeight: Kirigami.Units.gridUnit + bgLoader.topMargin + bgLoader.bottomMargin
        implicitWidth: Kirigami.Units.gridUnit + bgLoader.leftMargin + bgLoader.rightMargin
        height: parent.height
        hovered: control.up.hovered
        pressed: control.up.pressed
        focused: false
        checked: false
        Kirigami.Icon {
            anchors.centerIn: parent
            implicitWidth: Kirigami.Units.iconSizes.sizeForLabels
            implicitHeight: Kirigami.Units.iconSizes.sizeForLabels
            source: "spinbox-increase"
            fallback: "list-add"
        }
        KSvg.SvgItem {
            x: control.mirrored ? parent.width - width : 0
            z: -1
            anchors {
                top: parent.top
                bottom: parent.bottom
                topMargin: bgLoader.topMargin
                bottomMargin: bgLoader.bottomMargin
            }
            implicitWidth: naturalSize.width
            implicitHeight: implicitWidth
            elementId: "vertical-line"
            svg: lineSvg
        }
    }

    down.indicator: P.FlatButtonBackground {
        x: control.mirrored ? parent.width - width : 0
        implicitHeight: Kirigami.Units.gridUnit + bgLoader.topMargin + bgLoader.bottomMargin
        implicitWidth: Kirigami.Units.gridUnit + bgLoader.leftMargin + bgLoader.rightMargin
        height: parent.height
        hovered: control.down.hovered
        pressed: control.down.pressed
        focused: false
        checked: false
        Kirigami.Icon {
            anchors.centerIn: parent
            implicitWidth: Kirigami.Units.iconSizes.sizeForLabels
            implicitHeight: Kirigami.Units.iconSizes.sizeForLabels
            source: "spinbox-decrease"
            fallback: "list-remove"
        }
        KSvg.SvgItem {
            x: control.mirrored ? 0 : parent.width - width
            z: -1
            anchors {
                top: parent.top
                bottom: parent.bottom
                topMargin: bgLoader.topMargin
                bottomMargin: bgLoader.bottomMargin
            }
            implicitWidth: naturalSize.width
            implicitHeight: implicitWidth
            elementId: "vertical-line"
            svg: lineSvg
        }
    }

    contentItem: T.TextField {
        id: textField
        opacity: enabled ? 1 : 0.5
        implicitWidth: Math.ceil(contentWidth) + leftPadding + rightPadding
        implicitHeight: Math.ceil(contentHeight) + topPadding + bottomPadding
        text: control.displayText
        font: control.font
        Kirigami.Theme.colorSet: Kirigami.Theme.View
        Kirigami.Theme.inherit: false
        color: Kirigami.Theme.textColor
        selectionColor: Kirigami.Theme.highlightColor
        selectedTextColor: Kirigami.Theme.highlightedTextColor
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter
        readOnly: !control.editable
        validator: control.validator
        inputMethodHints: control.inputMethodHints
        selectByMouse: true
        hoverEnabled: false
    }

    background: Loader {
        id: bgLoader
        // Anchors are needed because the Loader tries to resize itself on load
        anchors {
            fill: parent
            topMargin: control.topInset
            // Anchors will automirrot, inset won't, so we wnt the left stays left regardless of the layout
            leftMargin: LayoutMirroring.enabled ? control.rightInset : control.leftInset
            rightMargin: LayoutMirroring.enabled ? control.leftInset : control.rightInset
            bottomMargin: control.bottomInset
        }
        readonly property real leftMargin: item.leftMargin
        readonly property real rightMargin: item.rightMargin
        readonly property real topMargin: item.topMargin
        readonly property real bottomMargin: item.bottomMargin
        sourceComponent: control.editable ? editableBg : noneditableBg
        Component {
            id: noneditableBg
            P.RaisedButtonBackground {
                hovered: control.hovered
                focused: control.visualFocus || (control.contentItem.activeFocus && (
                    control.contentItem.focusReason == Qt.TabFocusReason ||
                    control.contentItem.focusReason == Qt.BacktabFocusReason ||
                    control.contentItem.focusReason == Qt.ShortcutFocusReason
                ))
                checked: false
                pressed: false
            }
        }
        Component {
            id: editableBg
            KSvg.FrameSvgItem {
                readonly property real leftMargin: margins.left
                readonly property real rightMargin: margins.right
                readonly property real topMargin: margins.top
                readonly property real bottomMargin: margins.bottom
                imagePath: "widgets/lineedit"
                prefix: "base"
                KSvg.FrameSvgItem {
                    anchors {
                        fill: parent
                        leftMargin: -margins.left
                        topMargin: -margins.top
                        rightMargin: -margins.right
                        bottomMargin: -margins.bottom
                    }
                    imagePath: "widgets/lineedit"
                    prefix: "hover"
                    visible: opacity > 0
                    opacity: control.hovered
                    Behavior on opacity {
                        enabled: control.hovered && Kirigami.Units.longDuration > 0
                        NumberAnimation {
                            duration: Kirigami.Units.longDuration
                            easing.type: Easing.OutCubic
                        }
                    }
                }
                KSvg.FrameSvgItem {
                    property bool visualFocus: control.visualFocus || (control.contentItem.activeFocus
                        && (control.contentItem.focusReason == Qt.TabFocusReason ||
                            control.contentItem.focusReason == Qt.BacktabFocusReason ||
                            control.contentItem.focusReason == Qt.ShortcutFocusReason)
                    )
                    z: lineEditSvg.hasElement("hint-focus-over-base") ? 0 : -1
                    anchors {
                        fill: parent
                        leftMargin: -margins.left
                        topMargin: -margins.top
                        rightMargin: -margins.right
                        bottomMargin: -margins.bottom
                    }
                    imagePath: "widgets/lineedit"
                    prefix: visualFocus && lineEditSvg.hasElement("focusframe-center") ? "focusframe" : "focus"
                    visible: opacity > 0
                    opacity: visualFocus || control.activeFocus || control.contentItem.activeFocus
                    Behavior on opacity {
                        enabled: Kirigami.Units.longDuration > 0
                        NumberAnimation {
                            duration: Kirigami.Units.longDuration
                            easing.type: Easing.OutCubic
                        }
                    }
                }
                KSvg.Svg {
                    id: lineEditSvg
                    imagePath: "widgets/lineedit"
                }
            }
        }
    }
}
