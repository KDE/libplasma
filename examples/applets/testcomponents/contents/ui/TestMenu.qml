/*
    SPDX-FileCopyrightText: 2013 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick
import org.kde.plasma.components as PlasmaComponents

PlasmaComponents.Menu {
    id: testMenu

    PlasmaComponents.MenuItem {
        text: "Red Snapper"
        icon.name: "dragonplayer"
        onClicked: print("Clicked on: " + text)
    }

    PlasmaComponents.MenuItem {
        text: "Eel"
        icon.name: "animal"
        onClicked: print("Clicked on: " + text)
    }

    PlasmaComponents.MenuItem {
        text: "White Tip Reef Shark"
        icon.name: "kmag"
        onClicked: print("Clicked on: " + text)
    }

    Component.onCompleted: print("TestMenu.qml served .. opening")
}

