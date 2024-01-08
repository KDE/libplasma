/*
    SPDX-FileCopyrightText: 2013 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick

import org.kde.plasma.components as PlasmaComponents
import org.kde.kirigami as Kirigami

// VertexPage

PlasmaComponents.Page {
    id: examplesPage

    //property string shader
    property string pageName: "Shader Examples"
    property string pageDescription: "Shader Examples"
    property string icon: "weather-clear"

    anchors {
        fill: parent
        margins: _s
    }

    Kirigami.Heading {
        id: heading
        level: 1
        anchors {
            top: parent.top;
            left: parent.left
            right: parent.right
        }
        text: pageName
    }
    PlasmaComponents.Label {
        anchors {
            top: heading.bottom;
            left: parent.left;
            right: parent.right;
        }
        text: pageDescription
    }
}
