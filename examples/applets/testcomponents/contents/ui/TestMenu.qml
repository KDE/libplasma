/*
    SPDX-FileCopyrightText: 2013 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick
import org.kde.plasma.components as PlasmaComponents
import org.kde.kquickcontrolsaddons as KQuickControlsAddons

// PlasmoidPage

PlasmaComponents.Menu {
    id: testMenu

    PlasmaComponents.MenuItem {
        text: "Red Snapper"
        icon: "dragonplayer"
        onClicked: print(" Clicked on : " + text)
    }

    PlasmaComponents.MenuItem {
        text: "Eel"
        icon: "kthesaurus"
        onClicked: print(" Clicked on : " + text)
    }

    PlasmaComponents.MenuItem {
        text: "White Tip Reef Shark"
        icon: "kmag"
        onClicked: print(" Clicked on : " + text)
    }

    Component.onCompleted:{
        print("TestMenu.qml served .. opening");

    }
}

