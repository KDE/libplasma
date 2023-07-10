/*
    SPDX-FileCopyrightText: 2014 Vishesh Handa <vhanda@kde.org>
    SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick 2.0

import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.1

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.kirigami 2.20 as Kirigami

ColumnLayout
{
    height: Kirigami.Units.gridUnit * 20
    Controls.Label {
        id: label
        text: "Press the button and make sure the popup is on the correct place"
        wrapMode: Text.WordWrap
    }
    PlasmaComponents.Button {
        id: settingsButton
        iconSource: "configure"
        text: "Press Me"
        Layout.alignment: Qt.AlignHCenter
        rotation: 90

        onClicked: {
            contextMenu.visible = !contextMenu.visible;
        }
    }

    PlasmaCore.Dialog {
        id: contextMenu
        visualParent: settingsButton

        location: PlasmaCore.Types.BottomEdge
        type: PlasmaCore.Dialog.PopupMenu
        flags: Qt.Popup | Qt.FramelessWindowHint | Qt.WindowDoesNotAcceptFocus

        mainItem: ColumnLayout {
            id: menuColumn
            Layout.minimumWidth: menuColumn.implicitWidth
            Layout.minimumHeight: menuColumn.implicitHeight
            spacing: Kirigami.Units.smallSpacing

            PlasmaExtras.Heading {
                level: 3
                text: "Panel Alignment"
            }

            PlasmaComponents.ButtonColumn {
                spacing: 0
                Layout.fillWidth: true
                PlasmaComponents.ToolButton {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    text: "Left"
                    checkable: true
                    flat: false
                }
                PlasmaComponents.ToolButton {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    text: "Center"
                    checkable: true
                    flat: false
                }
            }
        }
    }
}
