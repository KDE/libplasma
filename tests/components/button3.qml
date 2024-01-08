/*
 * SPDX-FileCopyrightText: 2019 David Edmundson <kde@davidedmundson.co.uk>
 * SPDX-FileCopyrightText: 2020 Marco Martin <notmart@gmail.com>
 * SPDX-FileCopyrightText: 2020 Nate Graham <nate@kde.org>
 * SPDX-FileCopyrightText: 2020 Noah Davis <noahadvs@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */
import QtQuick
import QtQuick.Layouts
import org.kde.plasma.components as PlasmaComponents
import org.kde.kirigami as Kirigami

ComponentBase {
    id: root
    title: "PlasmaComponents 3 Button"
    contentItem: ColumnLayout {
        GridLayout {
            rowSpacing: Kirigami.Units.smallSpacing
            columnSpacing: Kirigami.Units.gridUnit
            columns: 2

            PlasmaComponents.Label {
                text: "icon + text"
            }

            PlasmaComponents.Button {
                icon.name: "start-here-kde-plasma"
                text: "test"
            }

            PlasmaComponents.Label {
                text: "icon alone, should look small and square"
            }

            PlasmaComponents.Button {
                icon.name: "start-here-kde-plasma"
            }

            PlasmaComponents.Label {
                text: "text alone, should be about as wide as the text itself"
            }

            PlasmaComponents.Button {
                text: "test"
            }

            PlasmaComponents.Label {
                text: "This should look highlighted on load"
            }

            PlasmaComponents.Button {
                text: "test"
                focus: true
            }

            PlasmaComponents.Label {
                text: "long text, should expand to fit"
            }

            PlasmaComponents.Button {
                icon.name: "start-here-kde-plasma"
                text: "This is a really really really really long button"
            }

            PlasmaComponents.Label {
                text: "long text but constrained, should be 150px and elided"
            }

            PlasmaComponents.Button {
                icon.name: "start-here-kde-plasma"
                text: "This is a really really really really long button"
                Layout.maximumWidth: 150
            }

            PlasmaComponents.Label {
                text: "disabled icon + text"
            }

            PlasmaComponents.Button {
                icon.name: "start-here-kde-plasma"
                text: "test"
                enabled: false
            }

            PlasmaComponents.Label {
                text: "button (with or without icon) and textfield should have the same height"
            }

            RowLayout {
                PlasmaComponents.Button {
                    text: "test"
                }
                PlasmaComponents.Button {
                    icon.name: "application-menu"
                    text: "test"
                }
                PlasmaComponents.TextField {
                }
            }

            PlasmaComponents.Label {
                text: "Display property"
            }
            RowLayout {
                PlasmaComponents.Button {
                    icon.name: "application-menu"
                    text: "Icon Only"
                    display: PlasmaComponents.Button.IconOnly
                }
                PlasmaComponents.Button {
                    icon.name: "application-menu"
                    text: "Text Beside Icon"
                    display: PlasmaComponents.Button.TextBesideIcon
                }
                PlasmaComponents.Button {
                    icon.name: "application-menu"
                    text: "Text Under Icon"
                    display: PlasmaComponents.Button.TextUnderIcon
                }
                PlasmaComponents.Button {
                    icon.name: "application-menu"
                    text: "Text Only"
                    display: PlasmaComponents.Button.TextOnly
                }
            }
        }
        Kirigami.Heading {
            text: "Flat Buttons"
        }
        GridLayout {
            rowSpacing: Kirigami.Units.smallSpacing
            columnSpacing: Kirigami.Units.gridUnit
            columns: 2

            PlasmaComponents.Label {
                text: "icon + text"
            }

            PlasmaComponents.Button {
                icon.name: "start-here-kde-plasma"
                text: "test"
                flat: true
            }

            PlasmaComponents.Label {
                text: "icon alone, should look small and square"
            }

            PlasmaComponents.Button {
                icon.name: "start-here-kde-plasma"
                flat: true
            }

            PlasmaComponents.Label {
                text: "text alone, should be about as wide as the text itself"
            }

            PlasmaComponents.Button {
                text: "test"
                flat: true
            }

            PlasmaComponents.Label {
                text: "long text, should expand to fit"
            }

            PlasmaComponents.Button {
                icon.name: "start-here-kde-plasma"
                text: "This is a really really really really long button"
                flat: true
            }

            PlasmaComponents.Label {
                text: "long text but constrained, should be 150px and elided"
            }

            PlasmaComponents.Button {
                icon.name: "start-here-kde-plasma"
                text: "This is a really really really really long button"
                Layout.maximumWidth: 150
                flat: true
            }

            PlasmaComponents.Label {
                text: "disabled icon + text"
            }

            PlasmaComponents.Button {
                icon.name: "start-here-kde-plasma"
                text: "test"
                flat: true
                enabled: false
            }

            PlasmaComponents.Label {
                text: "button (with or without icon) and textfield should have the same height"
            }

            RowLayout {
                PlasmaComponents.Button {
                    text: "test"
                    flat: true
                }
                PlasmaComponents.Button {
                    icon.name: "application-menu"
                    text: "test"
                    flat: true
                }
                PlasmaComponents.TextField {
                }
            }

            PlasmaComponents.Label {
                text: "Display property"
            }
            RowLayout {
                PlasmaComponents.Button {
                    icon.name: "application-menu"
                    text: "Icon Only"
                    display: PlasmaComponents.Button.IconOnly
                    flat: true
                }
                PlasmaComponents.Button {
                    icon.name: "application-menu"
                    text: "Text Beside Icon"
                    display: PlasmaComponents.Button.TextBesideIcon
                    flat: true
                }
                PlasmaComponents.Button {
                    icon.name: "application-menu"
                    text: "Text Under Icon"
                    display: PlasmaComponents.Button.TextUnderIcon
                    flat: true
                }
                PlasmaComponents.Button {
                    icon.name: "application-menu"
                    text: "Text Only"
                    display: PlasmaComponents.Button.TextOnly
                    flat: true
                }
            }
        }
    }
}

