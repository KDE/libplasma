/*
    SPDX-FileCopyrightText: 2014 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick 2.1

import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.kquickcontrolsaddons 2.0 as KQuickControlsAddons
import org.kde.kirigami 2.20 as Kirigami

// IconTab

Item {


    property int cheight: 48
    property int cwidth: themePage.width / 1.2

    id: themePage
    anchors {
        margins: Kirigami.Units.gridUnit
    }

    FontTest {
        anchors.fill: parent
    }

}
