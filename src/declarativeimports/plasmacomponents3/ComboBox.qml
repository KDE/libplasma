/*
    SPDX-FileCopyrightText: 2016 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls as Controls
import org.kde.ksvg as KSvg
//NOTE: importing PlasmaCore is necessary in order to make KSvg load the current Plasma Theme
import org.kde.plasma.core as PlasmaCore
import org.kde.kirigami as Kirigami
import "private" as Private
import "mobiletextselection" as MobileTextSelection

T.ComboBox {
    id: control

    property real __indicatorMargin: control.indicator && control.indicator.visible && control.indicator.width > 0 ? control.spacing + indicator.width : 0

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding)

    baselineOffset: contentItem.y + contentItem.baselineOffset

    hoverEnabled: true

    topPadding: surfaceNormal.margins.top
    leftPadding: surfaceNormal.margins.left + (!control.mirrored ? 0 : __indicatorMargin)
    rightPadding: surfaceNormal.margins.right + (control.mirrored ? 0 : __indicatorMargin)
    bottomPadding: surfaceNormal.margins.bottom
    spacing: Kirigami.Units.smallSpacing

    delegate: ItemDelegate {
        required property var model
        required property int index

        width: ListView.view.width
        text: model[control.textRole]
        highlighted: control.highlightedIndex == index
        property bool separatorVisible: false
    }

    indicator: KSvg.SvgItem {
        implicitWidth: Kirigami.Units.iconSizes.small
        implicitHeight: implicitWidth
        anchors {
            right: parent.right
            rightMargin: surfaceNormal.margins.right
            verticalCenter: parent.verticalCenter
        }
        svg: KSvg.Svg {
            imagePath: "widgets/arrows"
            colorSet: KSvg.Svg.Button
        }
        elementId: "down-arrow"
    }

    contentItem: T.TextField {
        id: textField
        implicitWidth: Math.ceil(contentWidth) + leftPadding + rightPadding
        implicitHeight: Math.ceil(contentHeight) + topPadding + bottomPadding
        padding: 0
        text: control.editable ? control.editText : control.displayText

        // We don't want a disabled text color when the control is not editable
        Kirigami.Theme.textColor: control.Kirigami.Theme.textColor
        enabled: control.editable
        autoScroll: control.editable

        readOnly: control.down || !control.editable
        inputMethodHints: control.inputMethodHints
        validator: control.validator
        color: Kirigami.Theme.textColor
        selectionColor: Kirigami.Theme.highlightColor
        selectedTextColor: Kirigami.Theme.highlightedTextColor

        selectByMouse: !Kirigami.Settings.tabletMode
        cursorDelegate: Kirigami.Settings.tabletMode ? mobileCursor : null

        font: control.font
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        opacity: control.enabled ? 1 : 0.3
        onFocusChanged: {
            if (focus) {
                MobileTextSelection.MobileTextActionsToolBar.controlRoot = textField;
            }
        }

        onTextChanged: MobileTextSelection.MobileTextActionsToolBar.shouldBeVisible = false;
        onPressed: event => MobileTextSelection.MobileTextActionsToolBar.shouldBeVisible = true;

        onPressAndHold: event => {
            if (!Kirigami.Settings.tabletMode) {
                return;
            }
            forceActiveFocus();
            cursorPosition = positionAt(event.x, event.y);
            selectWord();
        }
    }

    Component {
        id: mobileCursor
        MobileTextSelection.MobileCursor {
            target: textField
        }
    }

    MobileTextSelection.MobileCursor {
        target: textField
        selectionStartHandle: true
        property var rect: textField.positionToRectangle(textField.selectionStart)
        //FIXME: this magic values seem to be always valid, for every font,every dpi, every scaling
        x: rect.x + 5
        y: rect.y + 6
    }

    background: KSvg.FrameSvgItem {
        id: surfaceNormal

        anchors.fill: parent

        imagePath: control.editable ? "widgets/lineedit" : "widgets/button"
        prefix: control.editable
                ? "base"
                : (control.down ? "pressed" : "normal")

        Private.ButtonShadow {
            anchors.fill: parent
            showShadow: !control.editable && !control.down
        }

        Private.TextFieldFocus {
            visible: control.editable
            z: -1
            state: control.activeFocus ? "focus" : (control.enabled && control.hovered ? "hover" : "hidden")
            anchors.fill: parent
        }

        Private.ButtonFocus {
            anchors.fill: parent
            showFocus: control.activeFocus && !control.down
        }

        Private.ButtonHover {
            anchors.fill: parent
            showHover: control.enabled && control.hovered && !control.down
        }

        MouseArea {
            anchors {
                fill: parent
                leftMargin: control.leftPadding
                rightMargin: control.rightPadding
            }
            acceptedButtons: Qt.NoButton
            onWheel: wheel => {
                if (wheel.pixelDelta.y < 0 || wheel.angleDelta.y < 0) {
                    control.currentIndex = Math.min(control.currentIndex + 1, delegateModel.count -1);
                } else {
                    control.currentIndex = Math.max(control.currentIndex - 1, 0);
                }
                control.activated(control.currentIndex);
            }
        }
    }

    popup: T.Popup {
        x: control.mirrored ? control.width - width : 0
        y: control.height
        width: Math.max(control.width, 150)
        implicitHeight: contentItem.implicitHeight
        topMargin: 6
        bottomMargin: 6

        contentItem: ListView {
            id: listView
            clip: true
            implicitHeight: contentHeight
            model: control.popup.visible ? control.delegateModel : null
            currentIndex: control.highlightedIndex
            highlightRangeMode: ListView.ApplyRange
            highlightMoveDuration: 0
            // HACK: When the ComboBox is not inside a top-level Window, it's Popup does not inherit
            // the LayoutMirroring options. This is a workaround to fix this by enforcing
            // the LayoutMirroring options properly.
            // QTBUG: https://bugreports.qt.io/browse/QTBUG-66446
            LayoutMirroring.enabled: Qt.application.layoutDirection === Qt.RightToLeft
            LayoutMirroring.childrenInherit: true
            T.ScrollBar.vertical: Controls.ScrollBar { }
        }
        background: Kirigami.ShadowedRectangle {
            anchors {
                fill: parent
                margins: -1
            }
            radius: 2
            Kirigami.Theme.colorSet: Kirigami.Theme.View
            Kirigami.Theme.inherit: false
            color: Kirigami.Theme.backgroundColor
            border {
                color: Qt.rgba(Kirigami.Theme.textColor.r, Kirigami.Theme.textColor.g, Kirigami.Theme.textColor.b, 0.3)
                width: 1
            }
            shadow {
                size: 4
                xOffset: 2
                yOffset: 2
                color: Qt.rgba(0, 0, 0, 0.3)
            }
        }
    }
}
