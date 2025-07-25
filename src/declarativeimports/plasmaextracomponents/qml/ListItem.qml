/*
    SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

import QtQuick
import org.kde.plasma.components as PC3
import org.kde.kirigami as Kirigami
import org.kde.ksvg as KSvg

/*!
  \qmltype ListItem
  \inqmlmodule org.kde.plasma.extras

  TODO qdoc how to document inheritance
 */
PC3.ItemDelegate {
    id: __listItem

    /*!
      \qmlproperty list<Object> ListItem::content

      The content of this ListItem.
     */
    default property alias content: __innerItem.data

    /*!
      Whether this ListItem displays a separator.
     */
    property bool separatorVisible: true

    /*!
      Whether this ListItem renders as a section header.
     */
    property bool sectionDelegate: false

    /*!
      Whether this ListItem contains the mouse.
     */
    readonly property bool containsMouse: hovered

    leftPadding: __background.margins.left
    rightPadding: __background.margins.right
    topPadding: __background.margins.top
    bottomPadding: __background.margins.bottom

    // TODO KF6: `implicitContentWidth, implicitBackgroundWidth, leftInset and rightInset are not available in Controls 2.2 which this component is based on.
    implicitWidth: (contentItem ? contentItem.implicitWidth : 0) + leftPadding + rightPadding

    // TODO KF6: Make this behave more like the normal Control default.
    // Behaving this way for backwards compatibility reasons.
    contentItem: Item {
        id: __innerItem
    }

    background: KSvg.FrameSvgItem {
        id: __background
        imagePath: "widgets/listitem"
        prefix: (__listItem.sectionDelegate ? "section" :
                (__listItem.pressed || __listItem.checked) ? "pressed" : "normal")

        anchors.fill: parent
        visible: __listItem.ListView.view ? __listItem.ListView.view.highlight === null : true

        KSvg.SvgItem {
            svg: KSvg.Svg {
                imagePath: "widgets/listitem"
            }
            elementId: "separator"
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }
            height: naturalSize.height
            visible: __listItem.separatorVisible && (__listItem.sectionDelegate || (typeof(index) != "undefined" && index > 0 && !__listItem.checked && !__listItem.pressed))
        }
    }
}
