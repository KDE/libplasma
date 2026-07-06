/*
    SPDX-FileCopyrightText: 2013 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick
import QtQuick.Layouts

import org.kde.plasma.plasmoid
import org.kde.plasma.components as PlasmaComponents
import org.kde.kirigami as Kirigami

PlasmaComponents.Page {
    id: plasmoidPage

    ColumnLayout {
        anchors.fill: parent
        Kirigami.Heading {
            Layout.alignment: Qt.AlignHCenter
            level: 2
            text: "I'm an applet"
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }

        // PlasmaComponents.ButtonColumn {
        //     PlasmaComponents.RadioButton {
        //         text: "No background"
        //         onClicked: {
        //             if (checked) Plasmoid.backgroundHints = 0;
        //         }
        //     }
        //     PlasmaComponents.RadioButton {
        //         text: "Default background"
        //         checked: true
        //         onClicked: {
        //             if (checked) Plasmoid.backgroundHints = 1;
        //         }
        //     }
        //     PlasmaComponents.RadioButton {
        //         text: "Translucent background"
        //         onClicked: {
        //             if (checked) Plasmoid.backgroundHints = 2;
        //         }
        //     }
        // }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            PlasmaComponents.Button {
                text: "Busy"
                checked: Plasmoid.busy
                onClicked: {
                    Plasmoid.busy = !Plasmoid.busy
                }
            }

            PlasmaComponents.Button {
                id: ctxButton
                text: "Context Menu"

                TestMenu {
                    id: menu
                }
                onClicked: {
                    menu.open();
                }
            }
        }
    }
}

