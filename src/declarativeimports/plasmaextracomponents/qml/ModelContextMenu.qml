/*
    SPDX-FileCopyrightText: 2014 David Edmundson <davidedmundson@kde.org>
    SPDX-FileCopyrightText: 2017 Kai Uwe Broulik <kde@privat.broulik.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

import org.kde.plasma.extras as PlasmaExtras

import QtQuick
import QtQml

/*!
  \qmltype ModelContextMenu
  \inqmlmodule org.kde.plasma.extras

  \brief A ModelContextMenu creates a context menu with items populated from a model or a QList<QAction*>.

  For standard item models, actions are created using the following model role names or properties:
  \list
  \li display - a string contains the action name
  \li decoration - an icon to display
  \li separator - boolean that will add a separator in the list
  \endlist


  Example code:

  \qml
  ModelContextMenu {
      id: menu
      visualParent: someButton
      model: myModel
  }

  Button {
       id: someButton
       onClicked: menu.popup()
  }
  \endqml
 */

PlasmaExtras.Menu {
    id: menu

    /*!
      \qmlproperty model ModelContextMenu::model

      The model containing menu items
     */
    property alias model: instantiator.model

    /*!
       This signal is emitted when a menu item is clicked.
       The attached model properties for that menu item are passed as an argument
     */
    signal clicked(var model)

    //ContextMenu cannot have child items, so in order to have ContextMenu as the root object of this item
    //we create a new property which contains an item which can then load the child items
    property Instantiator _children: Instantiator {
        id: instantiator
        delegate: PlasmaExtras.MenuItem {
            //for QList<QAction*> Repeater adds an attached property modelData
            //for QAbstractItemModel* it doesn't. Not checking causes errors
            text: (typeof(modelData) != "undefined" ? modelData.text : model.display) || ""
            icon: typeof(modelData) != "undefined" ? modelData.icon : model.decoration
            separator: (typeof(modelData) != "undefined" ? modelData.separator : model.separator === true) || false
            section: (typeof(modelData) != "undefined" ? modelData.section : model.section === true) || false
            onClicked: {
                menu.clicked(typeof(modelData) != "undefined" ? modelData : model)
            }
        }

        onObjectAdded: (index, object) => menu.addMenuItem(object, null)
        onObjectRemoved: (index, object) => menu.removeMenuItem(object)
    }
}
