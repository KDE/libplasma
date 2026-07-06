/*
    SPDX-FileCopyrightText: 2013 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Window

import org.kde.plasma.components as PlasmaComponents
import org.kde.kirigami as Kirigami

PlasmaComponents.Page {
    id: root

    ColumnLayout {
        anchors.fill: parent
        Kirigami.Heading {
            Layout.fillWidth: true
            text: "Dialogs"
        }
        RowLayout {
            Layout.fillWidth: true
            PlasmaComponents.Button {
                id: windowButton
                checkable: true
                icon.source: "dialog-ok"
                text: "Window"
            }
            Window {
                title: windowButton.text
                id: qWindow
                visible: windowButton.checked
                color: Kirigami.Theme.backgroundColor
                onClosing: windowButton.checked = false
                DialogContent {
                    anchors.fill: parent
                    onCloseMe: windowButton.checked = false
                }
            }

            PlasmaComponents.Label {
                text: qWindow.visible ? "shown" : "hidden"
            }
        }
        RowLayout {
            Layout.fillWidth: true
            PlasmaComponents.Button {
                id: dialogButton
                text: "PlasmaComponents.Dialog"
                icon.source: "dialog-ok-apply"
                checkable: true
            }
            PlasmaComponents.Label {
                text: pcDialog.visible ? "shown" : "hidden"
            }

            PlasmaComponents.Dialog {
                id: pcDialog
                visible: dialogButton.checked
                onVisibleChanged:  if (!visible) dialogButton.checked = false

                contentItem: DialogContent {
                    onCloseMe: pcDialog.visible = false
                }
            }
        }
    }
}

