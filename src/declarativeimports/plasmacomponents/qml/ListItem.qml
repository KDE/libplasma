/*
 *   Copyright 2010 Marco Martin <notmart@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  2.010-1301, USA.
 */

import QtQuick 2.0
import org.kde.plasma.core 2.0 as PlasmaCore
import "private/Config.js" as Config

/**
 * An item delegate for the primitive ListView component.
 *
 * It's intended to make all listviews look coherent.
 */
Item {
    id: listItem
    default property alias content: paddingItem.data

    /**
     * type:bool Holds if the item emits signals related to mouse interaction.
     *
     * The default value is false.
     */
    property alias enabled: itemMouse.enabled
    //item has been clicked or pressed+hold

    /**
     * This signal is emitted when there is a click.
     *
     * This is disabled by default, set enabled to true to use it.
     * @see enabled
     */
    signal clicked


    /**
     * The user pressed the item with the mouse and didn't release it for a
     * certain amount of time.
     *
     * This is disabled by default, set enabled to true to use it.
     * @see enabled
     */
    signal pressAndHold

    /**
     * If true makes the list item look as checked or pressed. It has to be set
     * from the code, it won't change by itself.
     */
    //plasma extension
    //always look pressed?
    property bool checked: false

    /**
     * If true the item will be a delegate for a section, so will look like a
     * "title" for the otems under it.
     */
    //is this to be used as section delegate?
    property bool sectionDelegate: false

    width: parent ? parent.width : childrenRect.width
    height: paddingItem.childrenRect.height + background.margins.top + background.margins.bottom

    property int implicitHeight: paddingItem.childrenRect.height + background.margins.top + background.margins.bottom


    Connections {
        target: listItem
        onCheckedChanged: background.prefix = (listItem.checked ? "pressed" : "normal")
        onSectionDelegateChanged: background.prefix = (listItem.sectionDelegate ? "section" : "normal")
    }

    PlasmaCore.FrameSvgItem {
        id : background
        imagePath: "widgets/listitem"
        prefix: "normal"

        anchors.fill: parent
        visible: listItem.ListView.view ? listItem.ListView.view.highlight === null : true
        opacity: itemMouse.containsMouse && !itemMouse.pressed ? 0.5 : 1
        Component.onCompleted: {
            prefix = (listItem.sectionDelegate ? "section" : (listItem.checked ? "pressed" : "normal"))
        }
        Behavior on opacity { NumberAnimation { duration: 200 } }
    }
    PlasmaCore.SvgItem {
        svg: PlasmaCore.Svg {imagePath: "widgets/listitem"}
        elementId: "separator"
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        height: naturalSize.height
        visible: listItem.sectionDelegate || (typeof(index) != "undefined" && index > 0 && !listItem.checked && !itemMouse.pressed)
    }

    MouseArea {
        id: itemMouse
        property bool changeBackgroundOnPress: !listItem.checked && !listItem.sectionDelegate
        anchors.fill: background
        enabled: false
        hoverEnabled: Config.mouseOverEnabled

        onClicked: listItem.clicked()
        onPressAndHold: listItem.pressAndHold()
        onPressed: if (changeBackgroundOnPress) background.prefix = "pressed"
        onReleased: if (changeBackgroundOnPress) background.prefix = "normal"
        onCanceled: if (changeBackgroundOnPress) background.prefix = "normal"
    }

    Item {
        id: paddingItem
        anchors {
            fill: background
            leftMargin: background.margins.left
            topMargin: background.margins.top
            rightMargin: background.margins.right
            bottomMargin: background.margins.bottom
        }
    }
}
