/*
 * SPDX-FileCopyrightText: 2016 Marco Martin <mart@kde.org>
 * SPDX-FileCopyrightText: 2020 Noah Davis <noahadvs@gmail.com>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Templates as T
import org.kde.ksvg as KSvg
import org.kde.kirigami as Kirigami
import "../" as PlasmaComponents

RowLayout {
    id: root

    required property string labelText
    required property T.Button button

    readonly property bool usingFocusBackground: !button.flat && buttonSvg.hasElement("hint-focus-highlighted-background") && button.visualFocus && !(button.pressed || button.checked)
    readonly property int defaultIconSize: button.flat ? Kirigami.Units.iconSizes.smallMedium : Kirigami.Units.iconSizes.small

    spacing: button.spacing

    GridLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true
        columns: root.button.display == T.Button.TextBesideIcon ? 2 : 1
        rowSpacing: root.button.spacing
        columnSpacing: rowSpacing

        Kirigami.Icon {
            id: icon

            Layout.alignment: Qt.AlignCenter

            Layout.fillWidth: root.button.display !== T.Button.TextBesideIcon || root.labelText.length === 0
            Layout.fillHeight: true

            Layout.minimumWidth: Math.min(root.width, root.height, implicitWidth)
            Layout.minimumHeight: Math.min(root.width, root.height, implicitHeight)

            Layout.maximumWidth: root.button.icon.width > 0 ? root.button.icon.width : Number.POSITIVE_INFINITY
            Layout.maximumHeight: root.button.icon.height > 0 ? root.button.icon.height : Number.POSITIVE_INFINITY

            implicitWidth: root.button.icon.width > 0 ? root.button.icon.width : root.defaultIconSize
            implicitHeight: root.button.icon.height > 0 ? root.button.icon.height : root.defaultIconSize
            visible: source.length > 0 && root.button.display !== T.Button.TextOnly
            source: root.button.icon ? (root.button.icon.name || root.button.icon.source) : ""
            selected: root.usingFocusBackground
        }
        PlasmaComponents.Label {
            id: label
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: text.length > 0 && root.button.display !== T.Button.IconOnly
            text: root.labelText
            font: root.button.font
            color: root.usingFocusBackground ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor
            horizontalAlignment: root.button.display !== T.Button.TextUnderIcon && icon.visible ? Text.AlignLeft : Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
    }
    KSvg.SvgItem {
        visible: root.button.Accessible.role === Accessible.ButtonMenu && label.visible
        Layout.preferredWidth: Kirigami.Units.iconSizes.small
        Layout.preferredHeight: Layout.preferredWidth
        Layout.alignment: Qt.AlignCenter
        imagePath: "widgets/arrows"
        elementId: "down-arrow"
    }
    KSvg.Svg {
        id: buttonSvg
        imagePath: "widgets/button"
    }
}
