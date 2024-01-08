/*
 * SPDX-FileCopyrightText: 2019 David Edmundson <kde@davidedmundson.co.uk>
 * SPDX-FileCopyrightText: 2020 Marco Martin <notmart@gmail.com>
 * SPDX-FileCopyrightText: 2020 Noah Davis <noahadvs@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */
import QtQuick
import QtQuick.Layouts
import org.kde.plasma.components as PlasmaComponents
import org.kde.kirigami as Kirigami

ComponentBase {
    id: root
    title: "Plasma Components 3 ToolButton"

    contentItem: ColumnLayout {
        Flow {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: Kirigami.Units.gridUnit

            PlasmaComponents.ToolButton {
                icon.name: "start-here-kde-plasma"
                text: "test"
                flat: true
            }
            PlasmaComponents.ToolButton {
                icon.name: "start-here-kde-plasma"
                flat: true
            }
            PlasmaComponents.ToolButton {
                text: "test"
                flat: true
            }
            PlasmaComponents.ToolButton {
                icon.name: "start-here-kde-plasma"
                text: "test"
                flat: false
            }
            PlasmaComponents.ToolButton {
                icon.name: "start-here-kde-plasma"
                flat: false
            }
            PlasmaComponents.ToolButton {
                text: "test"
                flat: false
            }
            PlasmaComponents.ToolButton {
                icon.name: "application-menu"
                text: "Icon Only"
                display: PlasmaComponents.ToolButton.IconOnly
            }
            PlasmaComponents.ToolButton {
                icon.name: "application-menu"
                text: "Text Beside Icon"
                display: PlasmaComponents.ToolButton.TextBesideIcon
            }
            PlasmaComponents.ToolButton {
                icon.name: "application-menu"
                text: "Text Under Icon"
                display: PlasmaComponents.ToolButton.TextUnderIcon
            }
            PlasmaComponents.ToolButton {
                icon.name: "application-menu"
                text: "Text Only"
                display: PlasmaComponents.ToolButton.TextOnly
            }
        }
        RowLayout {
            Layout.fillWidth: true
            PlasmaComponents.Label {
                Layout.fillWidth: true
                text: "They should always be square:"
            }
            PlasmaComponents.ToolButton {
                icon.name: "start-here-kde-plasma"
            }
            PlasmaComponents.ToolButton {
                icon.name: "start-here-kde-plasma"
            }
            PlasmaComponents.ToolButton {
                icon.name: "start-here-kde-plasma"
            }
            PlasmaComponents.ToolButton {
                icon.name: "start-here-kde-plasma"
            }
        }
        PlasmaComponents.Label {
            text: "Fixed size and stretching size buttons"
        }
        GridLayout {
            id: layout
            rows: 2
            columns:2
            Layout.fillWidth: true
            Layout.fillHeight: true
            PlasmaComponents.ToolButton {
                id: closeButton
                icon.name: "window-close"
                text: "Text"
            }
            PlasmaComponents.ToolButton {
                id: closeButton2
                icon.name: "window-close"
                Layout.fillWidth: true
                Layout.fillHeight: true
                icon.width: Kirigami.Units.iconSizes.small
                icon.height: Kirigami.Units.iconSizes.small
                text: "Text"
            }
            PlasmaComponents.ToolButton {
                id: closeButton3
                icon.name: "window-close"
                Layout.fillHeight: true
                text: "Text"
            }
            PlasmaComponents.ToolButton {
                id: closeButton4
                icon.name: "window-close"
                Layout.fillWidth: true
                Layout.fillHeight: true
                text: "Text"
            }
        }
    }
}

