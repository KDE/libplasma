/*
    SPDX-FileCopyrightText: 2013 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick
import QtQuick.Layouts

import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.components as PlasmaComponents
import org.kde.kirigami as Kirigami

PlasmaComponents.Page {
    id: root

    ColumnLayout {
        anchors.fill: parent

        Kirigami.Heading {
            Layout.fillWidth: true
            elide: Text.ElideRight
            level: 1
            text: "Icons"
        }
        PlasmaComponents.Label {
            Layout.fillWidth: true
            wrapMode: PlasmaComponents.Label.Wrap
            text: "iconSizes.small  : " + Kirigami.Units.iconSizes.small +
                        "\niconSizes.smallMedium: " + Kirigami.Units.iconSizes.medium +
                        "\niconSizes.medium: " + Kirigami.Units.iconSizes.medium +
                        "\niconSizes.large: " + Kirigami.Units.iconSizes.medium +
                        "\niconSizes.huge: " + Kirigami.Units.iconSizes.medium +
                        "\niconSizes.enormous: " + Kirigami.Units.iconSizes.medium

        }
        Flow {
            Layout.fillWidth: true

            Kirigami.Icon {
                source: "configure"
            }
            Kirigami.Icon {
                source: "dialog-ok"
            }
            Kirigami.Icon {
                source: "folder-green"
            }
            Kirigami.Icon {
                source: "akonadi"
            }
            Kirigami.Icon {
                source: "clock"
            }
            Kirigami.Icon {
                source: "preferences-desktop-icons"
            }

        }
        Kirigami.Heading {
            level: 2
            text: "ToolTip"
        }

        RowLayout {
            Layout.fillWidth: true
            PlasmaCore.ToolTipArea {
                Layout.preferredWidth: Kirigami.Units.gridUnit * 2
                Layout.preferredHeight: Kirigami.Units.gridUnit * 2
                icon: "klipper"
                mainText: "Fish sighted in the wild, in the wild, a fish was seen."
                subText: "A mean-looking grouper swam by."
                Kirigami.Icon {
                    anchors.fill: parent
                    objectName: "akonadiIcon"
                    source: "akonadi"
                    Rectangle { color: "orange"; opacity: 0.3; anchors.fill: parent; }
                }
            }
            PlasmaCore.ToolTipArea {
                image: bridgeimage.source
                Layout.preferredWidth: Kirigami.Units.gridUnit * 2
                Layout.preferredHeight: Kirigami.Units.gridUnit * 2
                mainText: "Bridge"
                subText: "Waalbrug."
                Image {
                    id: bridgeimage
                    anchors.fill: parent
                    objectName: "bridgeimage"
                    fillMode: Image.PreserveAspectFit
                    source: "../images/bridge.jpg"
                }
            }
            PlasmaCore.ToolTipArea {
                Layout.preferredWidth: Kirigami.Units.gridUnit * 2
                Layout.preferredHeight: Kirigami.Units.gridUnit * 2
                mainItem: PlasmaComponents.Label {
                    text: "Nijmegen North Beach\nA surfboard on the beach. The photo shows the Waal river's north beach, across the water from Nijmegen, Netherlands.\nIt was taken during the summer festivals a few years back."
                    anchors.centerIn: parent
                }
                Image {
                    anchors.fill: parent
                    objectName: "surfboardimage"
                    fillMode: Image.PreserveAspectFit
                    source: "../images/surfboard.jpg"

                }
            }
            PlasmaCore.ToolTipArea {
                Layout.preferredWidth: Kirigami.Units.gridUnit * 4
                Layout.preferredHeight: Kirigami.Units.gridUnit * 2
                mainText: "Tooltip on button"
                PlasmaComponents.Button {
                    anchors.fill: parent
                    text: "Button"
                    icon.source: "call-start"
                }
            }
        }
        Item {
            Layout.fillHeight: true
        }
    }
}
