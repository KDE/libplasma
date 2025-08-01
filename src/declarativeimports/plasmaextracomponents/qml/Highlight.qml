/*
    SPDX-FileCopyrightText: 2011 Daker Fernandes Pinheiro <dakerfp@gmail.com>
    SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
    SPDX-FileCopyrightText: 2023 ivan tkachenko <me@ratijas.tk>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

import QtQuick
import org.kde.ksvg as KSvg
import org.kde.kirigami as Kirigami

/*!
  \qmltype Highlight
  \inqmlmodule org.kde.plasma.extras

  \brief Highlight for a list or grid item.

  Highlight provides the highlight used to indicate the active
  item in a model view. It is typically used in conjunction with
  the ListView::highlight or the
  GridView::highlight properties.

  Provides built-in animation of Behavior on opacity Easing.OutQuad for a
  duration of 50ms (defined in Kirigami.Units.veryShortDuration).

  \qml
  import QtQuick
  import org.kde.plasma.extras as PlasmaExtras

  ListView {
      highlightFollowsCurrentItem: true
      highlight: PlasmaExtras.Highlight { }
      highlightMoveDuration: 0
      highlightResizeDuration: 0
      currentIndex: -1
  }

  \endqml
 */
Item {
    id: highlight

    /*!
      This property holds whether the control is hovered.

      This is set automatically when used in a ListView and GridView.
     */
    property bool hovered: ListView.view !== null || GridView.view !== null

    /*!
      This property holds whether the highlight has a pressed appearance.
     */
    property bool pressed: false

    /*!
      \qmlproperty int Highlight::marginHints

      This property holds the margin hints used by the background.
    */
    property alias marginHints: background.margins

    /*!
      This property holds whether the item is active. True by default. Set it to
      false to visually mark an item that's in the "current item" or "selected"
      state but is not currently being hovered.
     */
    property bool active: true

    width: {
        const view = ListView.view;
        return view ? view.width - view.leftMargin - view.rightMargin : undefined;
    }

    KSvg.FrameSvgItem {
        id: background

        anchors.fill: parent

        opacity: highlight.active ? 1 : 0.6

        imagePath: "widgets/viewitem"
        prefix: {
            if (highlight.pressed) {
                return highlight.hovered ? 'selected+hover' : 'selected';
            }

            return highlight.hovered ? 'hover' : 'normal';
        }

        Behavior on opacity {
            enabled: Kirigami.Units.veryShortDuration > 0
            NumberAnimation {
                duration: Kirigami.Units.veryShortDuration
                easing.type: Easing.OutQuad
            }
        }
    }
}
