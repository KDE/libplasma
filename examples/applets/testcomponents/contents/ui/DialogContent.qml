/*
    SPDX-FileCopyrightText: 2013 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick
import QtQuick.Window

import org.kde.plasma.components as PlasmaComponents
import org.kde.kquickcontrolsaddons as KQuickControlsAddons
import org.kde.kirigami as Kirigami

// DialogContent

Item {
    id: dialogsPage
    width: 300
    height: 200
    signal closeMe()
    Rectangle {
        color: "green"
        //anchors.margins: 24
        opacity: 0
        anchors.fill: parent
    }
    Column {
        anchors.fill: parent
        spacing: 12
        Kirigami.Heading {
            id: tx
            level: 1
            text: "Test Dialog"
        }
        PlasmaComponents.TextArea {
            anchors { left: parent.left; right: parent.right; top: localeItem.bottom; }
            width: parent.width
            height: 80
            wrapMode: TextEdit.Wrap
        }
        PlasmaComponents.Button {
            id: thanks
            anchors { horizontalCenter: parent.horizontalCenter; bottom: parent.bottom; bottomMargin: 24; }
            iconSource: "dialog-ok"
            text: "Thanks."
            //onClicked: dialogsPage.parent.visible = false;
            onClicked: closeMe()
        }
    }
}

