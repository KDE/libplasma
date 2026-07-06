/*
    SPDX-FileCopyrightText: 2013 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick
import QtQuick.Layouts

import org.kde.plasma.components as PlasmaComponents
import org.kde.kirigami as Kirigami

PlasmaComponents.Page {
    id: root

    ColumnLayout {
        anchors.fill: parent

        Kirigami.Heading {
            Layout.fillWidth: true
            text: "Buttons"
        }
        RowLayout {
            Layout.fillWidth: true
            PlasmaComponents.Button {
                text: "Button"
                icon.source: "call-start"
            }
            PlasmaComponents.ToolButton {
                text: "ToolButton"
                icon.source: "call-stop"
            }
        }
        RowLayout {
            Layout.fillWidth: true
            PlasmaComponents.RadioButton {
                id: radio
                text: "RadioButton"
                icon.source: "call-stop"
                onCheckedChanged: if (checked) textField.forceActiveFocus()
            }
            PlasmaComponents.TextField {
                id: textField
                enabled: radio.checked
                text: "input here"
                clearButtonShown: true
            }
        }
    }
}

