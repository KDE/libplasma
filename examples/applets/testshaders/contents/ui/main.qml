/*
    SPDX-FileCopyrightText: 2013 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick
import QtQuick.Layouts

import org.kde.plasma.components as PlasmaComponents
import org.kde.kirigami as Kirigami

Item {
    id: root
    width: 400
    height: 400

    Layout.minimumWidth: Kirigami.Units.gridUnit * 20
    Layout.minimumHeight: Kirigami.Units.gridUnit * 30
    property int _s: Kirigami.Units.iconSizes.small
    property int _h: Kirigami.Units.iconSizes.desktop
    property int _m: 12

    Item {
        id: mainItem
        anchors.fill: parent

        PlasmaComponents.TabBar {
            id: tabBar

            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }
            height: Kirigami.Units.iconSizes.toolbar * 1.5

            PlasmaComponents.TabButton { tab: colorShower; text: tab.pageName; }
            PlasmaComponents.TabButton { tab: wobbleExample; text: tab.pageName; }
            PlasmaComponents.TabButton { tab: shadowExample; text: tab.pageName; }
            PlasmaComponents.TabButton { tab: simpleExample; text: tab.pageName; }
            //PlasmaComponents.TabButton { tab: vertexPage; iconSource: vertexPage.icon; }
        }

        PlasmaComponents.TabGroup {
            id: tabGroup
            anchors {
                left: parent.left
                right: parent.right
                top: tabBar.bottom
                bottom: parent.bottom
            }

            ColorShower {
                id: colorShower
            }
            WobbleExample {
                id: wobbleExample
            }
//             ColorExample {
//                 id: colorExample
//             }
            Shadows {
                id: shadowExample
            }
            SimpleExample {
                id: simpleExample
            }
//             EditorPage {
//                 id: vertexPage
//             }
        }
    }

    Component.onCompleted: {
        print("Shader Test Applet loaded");
    }
}
