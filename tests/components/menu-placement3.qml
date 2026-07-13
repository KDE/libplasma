/*
 * SPDX-FileCopyrightText: 2015 Kai Uwe Broulik <kde@privat.broulik.de>
 * SPDX-FileCopyrightText: 2019 David Edmundson <kde@davidedmundson.co.uk>
 * SPDX-FileCopyrightText: 2020 Noah Davis <noahadvs@gmail.com>
 * SPDX-FileCopyrightText: 2023 ivan tkachenko <me@ratijas.tk>
 * SPDX-FileCopyrightText: 2026 Filip Fila <filip.fila@oxygen-design.org>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */
import QtQuick
import org.kde.plasma.components as PC3
import org.kde.kirigami as Kirigami

ComponentBase {
    id: root

    title: "PC3 Menu Placement"

    // TODO: There is no "placement" property in PC3.Menu right now
    // so this is reverse engineered behavior, but it should
    // ideally and eventually be added to the component instead of here
    property string currentPlacement: "BottomPosedLeftAlignedPopup"
    function placementOffset(name, btnW, btnH, menuW, menuH) {
        switch (name) {
        case "TopPosedLeftAlignedPopup":     return Qt.point(0, -menuH);
        case "TopPosedRightAlignedPopup":    return Qt.point(btnW - menuW, -menuH);
        case "LeftPosedTopAlignedPopup":     return Qt.point(-menuW, 0);
        case "LeftPosedBottomAlignedPopup":  return Qt.point(-menuW, btnH - menuH);
        case "BottomPosedLeftAlignedPopup":  return Qt.point(0, btnH);
        case "BottomPosedRightAlignedPopup": return Qt.point(btnW - menuW, btnH);
        case "RightPosedTopAlignedPopup":    return Qt.point(btnW, 0);
        case "RightPosedBottomAlignedPopup": return Qt.point(btnW, btnH - menuH);
        default:                             return Qt.point(0, btnH);
        }
    }

    // PlasmaExtras.Menu's "visualParent" becomes "parent"
    PC3.Menu {
        id: menu
        parent: centralButton

        readonly property point offset: root.placementOffset(
            root.currentPlacement, centralButton.width, centralButton.height,
            implicitWidth, implicitHeight)

        x: offset.x
        y: offset.y

        PC3.MenuItem { text: "Hello"; enabled: false /* TODO: missing a "section: true" property, see the menu3.qml test */ }
        PC3.MenuItem { text: "This is just a simple" }
        PC3.MenuItem { text: "Menu" }
        PC3.MenuSeparator {}
        PC3.MenuItem { text: "With separators" }
        PC3.MenuItem { text: "And other stuff" }
    }

    component PlacementRadioButton : PC3.RadioButton {
        required property string placementName

        anchors.margins: Kirigami.Units.gridUnit
        text: placementName.replace("Popup", "").replace(/([a-z])([A-Z])/g, "$1 $2")
        checked: root.currentPlacement === placementName
        onToggled: root.currentPlacement = placementName
    }

    contentItem: Item {
        implicitWidth: 600
        implicitHeight: 500

        PC3.Button {
            id: centralButton
            anchors.centerIn: parent
            width: 200
            height: 200
            text: "Open Relative"
            onClicked: {
                if (menu.visible) {
                    menu.dismiss();
                } else {
                    menu.open();
                }
            }
        }

        Repeater {
            model: [
                { placementName: "TopPosedLeftAlignedPopup", bottom: "top", right: "horizontalCenter" },
                { placementName: "TopPosedRightAlignedPopup", bottom: "top", left: "horizontalCenter" },
                { placementName: "LeftPosedTopAlignedPopup", right: "left", bottom: "verticalCenter" },
                { placementName: "LeftPosedBottomAlignedPopup", right: "left", top: "verticalCenter" },
                { placementName: "BottomPosedLeftAlignedPopup", top: "bottom", right: "horizontalCenter" },
                { placementName: "BottomPosedRightAlignedPopup", top: "bottom", left: "horizontalCenter"  },
                { placementName: "RightPosedTopAlignedPopup", left: "right", bottom: "verticalCenter" },
                { placementName: "RightPosedBottomAlignedPopup", left: "right", top: "verticalCenter" },
            ]
            PlacementRadioButton {
                required property var modelData
                placementName: modelData.placementName
                anchors.top: centralButton[modelData.top]
                anchors.left: centralButton[modelData.left]
                anchors.right: centralButton[modelData.right]
                anchors.bottom: centralButton[modelData.bottom]
            }
        }
    }

    // TODO? No footer like in the PlasmaExtras.Menu because PC3.Menu has no "preferSeamlessEdges" property
}
