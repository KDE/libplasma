/*
    SPDX-FileCopyrightText: 2013 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick
import QtQuick.Layouts

import org.kde.plasma.components as PlasmaComponents
import org.kde.plasma.extras as PlasmaExtras
import org.kde.kquickcontrolsaddons as KQuickControlsAddons
import org.kde.kirigami as Kirigami

// MousePage

PlasmaComponents.Page {
    id: mousePage

    Kirigami.Heading {
        id: mellabel
        level: 1
        text: "MouseEventListener"
        anchors { left: parent.left; right: parent.right; top: parent.top }
    }
    KQuickControlsAddons.MouseEventListener {
        id: mel
        hoverEnabled: true
        anchors { left: parent.left; right: parent.right; top: mellabel.bottom; bottom: parent.bottom; }

        onPressed: {
            print("Pressed");
            melstatus.text = "pressed";
        }
        onPositionChanged: mouse => print("positionChanged: " + mouse.x + "," + mouse.y)

        onReleased: {
            print("Released");
            melstatus.text = "Released";
        }
        onPressAndHold: {
            print("pressAndHold");
            melstatus.text = "pressAndHold";
        }
        onClicked: {
            print("Clicked");
            melstatus.text = "clicked";
        }
        onWheelMoved: wheel => print("Wheel: " + wheel.delta)

        onContainsMouseChanged: print("Contains mouse: " + containsMouse)

        ColumnLayout {
            anchors.fill: parent
            PlasmaComponents.Button {
                text: "Button"
                icon.source: "call-start"
            }
            PlasmaComponents.ToolButton {
                text: "ToolButton"
                icon.source: "call-stop"
            }
            PlasmaComponents.RadioButton {
                text: "RadioButton"
                icon.source: "call-stop"
            }
            PlasmaComponents.Label {
                id: melstatus
            }
        }

    }
}

