/*
    SPDX-FileCopyrightText: 2013 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Window

import org.kde.plasma.components as PlasmaComponents
import org.kde.kirigami as Kirigami

Item {
    id: root

    signal closeMe()

    ColumnLayout {
        anchors.fill: parent

        Kirigami.Heading {
            Layout.alignment: Qt.AlignHCenter
            level: 1
            text: "Test Dialog"
        }
        PlasmaComponents.TextArea {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 100
            Layout.preferredHeight: 50
            wrapMode: TextEdit.Wrap
        }
        PlasmaComponents.Button {
            id: thanks
            Layout.alignment: Qt.AlignHCenter
            icon.source: "dialog-ok"
            text: "Thanks."
            onClicked: root.closeMe()
        }
    }
}

