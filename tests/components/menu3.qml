/*
 * SPDX-FileCopyrightText: 2015 Kai Uwe Broulik <kde@privat.broulik.de>
 * SPDX-FileCopyrightText: 2019 David Edmundson <kde@davidedmundson.co.uk>
 * SPDX-FileCopyrightText: 2020 Noah Davis <noahadvs@gmail.com>
 * SPDX-FileCopyrightText: 2026 Filip Fila <filip.fila@oxygen-design.org>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */
import QtQuick
import QtQuick.Layouts
import org.kde.plasma.components as PC3
import org.kde.kirigami as Kirigami

ComponentBase {
    id: root
    title: "PC3 Menu"
    contentItem: ColumnLayout {
        spacing: Kirigami.Units.gridUnit

        PC3.Button {
            text: "Simple menu"
            onClicked: simpleMenu.open()

            PC3.Menu {
                id: simpleMenu
                // TODO: there's no "placement" property for PC3 Menu so we have to set the y for now
                y: parent.height

                PC3.MenuItem { text: "Hello" }
                PC3.MenuItem { text: "This is just a simple" }
                PC3.MenuItem { text: "Menu" }
                PC3.MenuItem { text: "without separators" }
                PC3.MenuItem { text: "and other stuff" }
            }
        }

        PC3.Button {
            text: "Checkable menu items"
            onClicked: checkableMenu.open()

            PC3.Menu {
                id: checkableMenu
                y: parent.height

                // FIXME? Menu disappears when item is checked
                PC3.MenuItem { text: "Apple"; checkable: true }
                PC3.MenuItem { text: "Banana"; checkable: true }
                PC3.MenuItem { text: "Orange"; checkable: true }
            }
        }


        PC3.Button {
            text: "Icons"
            onClicked: iconsMenu.open()

            PC3.Menu {
                id: iconsMenu
                y: parent.height

                PC3.MenuItem { text: "Error"; icon.name: "dialog-error" }
                PC3.MenuItem { text: "Warning"; icon.name: "dialog-warning" }
                PC3.MenuItem { text: "Information"; icon.name: "dialog-information" }
            }
        }

        PC3.Button {
            text: "Separators"
            onClicked: sectionsMenu.open()

            PC3.Menu {
                id: sectionsMenu
                y: parent.height

                // TODO: PC3.MenuItem currently doesn't have a "section" property
                PC3.MenuItem { text: "A menu"; enabled: false }
                PC3.MenuItem { text: "One entry" }
                PC3.MenuItem { text: "Another entry" }
                PC3.MenuSeparator {}
                PC3.MenuItem { text: "One item" }
                PC3.MenuItem { text: "Another item" }
            }
        }

        RowLayout {
            spacing: Kirigami.Units.smallSpacing

            PC3.Button {
                id: minMaxButton
                text: "Fixed minimum and maximum width"
                onClicked: minMaxMenu.open()

                PC3.Menu {
                    id: minMaxMenu
                    y: parent.height

                    // TODO: PC3.Menu currently has no "minimumWidth/maximumWidth" properties
                    readonly property real minimumWidth: minMaxButton.width
                    readonly property real maximumWidth: implicitWidth

                    width: limitMenuMaxWidth.checked ? minimumWidth : maximumWidth

                    PC3.MenuItem { text: "Hello" }
                    PC3.MenuItem { text: "This is just a simple" }
                    PC3.MenuItem { text: "Menu" }
                    PC3.MenuItem { text: "with some very very long text in one item that will "
                                                    + "make the menu super huge if you don't do anything about it" }
                    PC3.MenuItem { text: "and other stuff" }
                }
            }

            PC3.CheckBox {
                id: limitMenuMaxWidth
                Layout.alignment: Qt.AlignVCenter
                text: "Limit maximum width"
                checked: true
            }
        }

        PC3.Button {
            text: "Don't crash on null MenuItem action"
            onClicked: noActionCrashMenu.open()

            PC3.Menu {
                id: noActionCrashMenu
                y: parent.height

                PC3.MenuItem { text: "This is an item" }
                PC3.MenuItem { text: "Below me should NOT be an empty item"}
                //TODO: PC3.Menu *does* show empty menu entries
                PC3.MenuItem { action: null }
                PC3.MenuItem { text: "I am not empty" }
            }
        }

        PC3.Button {
            text: "Disabled menu items"
            onClicked: disabledMenuItemsMenu.open()

            PC3.Menu {
                id: disabledMenuItemsMenu
                y: parent.height

                PC3.MenuItem { text: "I'm disabled"; enabled: false }
                PC3.MenuItem { text: "And I'm enabled"}
                PC3.MenuItem { text: "I'm disabled too"; enabled: false }
            }
        }
    }
}
