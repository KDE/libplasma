/*
 *  Copyright 2013 Marco Martin <mart@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  2.010-1301, USA.
 */

import QtQuick 2.0

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents


Item {
    id: root
    width: 640
    height: 48

    property Item toolBox

    Connections {
        target: plasmoid
        onAppletAdded: {
            var container = appletContainerComponent.createObject((plasmoid.formFactor == 3) ? column : row)
            print("Applet added in test panel: " + applet)
            applet.parent = container
            container.applet = applet
            applet.anchors.fill = applet.parent
            applet.visible = true
            container.visible = true
        }
        onFormFactorChanged: {
            if (plasmoid.formFactor == 3) {
                for (var container in row.children) {
                    container.parent = column
                }
            } else {
                for (var container in column.children) {
                    container.parent = row
                }
            }
        }
    }

    Component {
        id: appletContainerComponent
        Item {
            id: container
            visible: false

            width: Math.min(root.width, root.height)
            height:  width

            property Item applet


            PlasmaComponents.BusyIndicator {
                z: 1000
                visible: applet && applet.length > 0 && applet[0].busy
                running: visible
                anchors.centerIn: parent
            }
        }
    }

    Row {
        id: row
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
        }
    }
    Column {
        id: column
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
    }

    Component.onCompleted: {
        print("Test Panel loaded")
        print(plasmoid)
    }
}
