/*
    SPDX-FileCopyrightText: 2014 Vishesh Handa <vhanda@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick

import QtQuick.Controls as Controls
import QtQuick.Layouts

import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.components as PC3
import org.kde.kirigami as Kirigami

ColumnLayout
{
    Controls.Label {
        text: "Press the button and make sure the popup is on the correct place"
        wrapMode: Text.WordWrap
    }
    PC3.Button {
        id: settingsButton
        icon.name: "configure"
        text: "Press Me"
        Layout.alignment: Qt.AlignHCenter

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

            Kirigami.Heading {
                level: 3
                text: "Panel Alignment"
            }

            Column {
                spacing: 5
                Layout.fillWidth: true
                PC3.ToolButton {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    text: "Left"
                    checkable: true
                    flat: false
                }
                PC3.ToolButton {
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
