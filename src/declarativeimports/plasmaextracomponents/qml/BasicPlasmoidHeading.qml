/*
    SPDX-FileCopyrightText: 2020 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Templates as T

import org.kde.plasma.plasmoid
import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.extras as PlasmaExtras
import org.kde.plasma.components as PlasmaComponents
import org.kde.kirigami as Kirigami

 /*!
   \qmltype BasicPlasmoidHeading
   \inqmlmodule org.kde.plasma.extras

   \brief A standard basic header for plasmoids which has title, a config button and
   a popup menu with all extra plasmoid actions.

   By default, it will be invisible when the plasmoid is in the system tray,
   as it provides a replacement header with the same features
  */
PlasmoidHeading {
    /*!
      \qmlproperty list<QtObject> BasicPlasmoidHeading::extraControls

      Any extra control and button that may be inserted in the heading
     */
    default property alias extraControls: extraControlsLayout.data

    visible: !(Plasmoid.containmentDisplayHints & PlasmaCore.Types.ContainmentDrawsPlasmoidHeading)

    contentItem: RowLayout {
        Heading {
            elide: Text.ElideRight
            wrapMode: Text.NoWrap
            Layout.fillWidth: true
            level: 1
            text: Plasmoid.title
        }
        RowLayout {
            id: extraControlsLayout
            visible: children.length > 0
            Layout.fillHeight: true
        }
        PlasmaComponents.ToolButton {
            id: actionsButton
            visible: visibleActions > 0
            checked: configMenu.status !== PlasmaExtras.Menu.Closed
            property int visibleActions: menuItemFactory.count
            property QtObject singleAction: visibleActions === 1 ? menuItemFactory.object.action : null
            icon.name: "open-menu-symbolic"
            checkable: visibleActions > 1
            contentItem.opacity: visibleActions > 1
            // NOTE: it needs an Icon because QtQuickControls2 buttons cannot load QIcons as their icon
            Kirigami.Icon {
                parent: actionsButton
                anchors.centerIn: parent
                active: actionsButton.hovered
                implicitWidth: Kirigami.Units.iconSizes.smallMedium
                implicitHeight: implicitWidth
                source: actionsButton.singleAction !== null ? actionsButton.singleAction.icon : ""
                visible: actionsButton.singleAction
            }
            onToggled: {
                if (checked) {
                    configMenu.openRelative();
                } else {
                    configMenu.close();
                }
            }
            onClicked: {
                if (singleAction) {
                    singleAction.trigger();
                }
            }
            PlasmaComponents.ToolTip {
                text: actionsButton.singleAction ? actionsButton.singleAction.text : i18nd("libplasma6", "More actions")
            }
            PlasmaExtras.Menu {
                id: configMenu
                visualParent: actionsButton
                placement: PlasmaExtras.Menu.BottomPosedLeftAlignedPopup
            }

            Instantiator {
                id: menuItemFactory
                model: {
                    configMenu.clearMenuItems();
                    const configureAction = Plasmoid.internalAction("configure");
                    const actions = Plasmoid.contextualActions
                        .filter(action => action !== configureAction);
                    return actions;
                }
                delegate: PlasmaExtras.MenuItem {
                    required property QtObject modelData // type: QAction
                    action: modelData
                }
                onObjectAdded: {
                    configMenu.addMenuItem(object);
                }
            }
        }
        PlasmaComponents.ToolButton {
            id: configureButton

            property PlasmaCore.Action internalAction

            function fetchInternalAction() {
                internalAction = Plasmoid.internalAction("configure");
            }

            Connections {
                target: Plasmoid
                function onInternalActionsChanged(actions) {
                    configureButton.fetchInternalAction();
                }
            }

            Component.onCompleted: fetchInternalAction()

            icon.name: "configure"
            visible: internalAction !== null
            text: internalAction?.text ?? ""
            display: T.AbstractButton.IconOnly
            PlasmaComponents.ToolTip {
                text: configureButton.text
            }
            onClicked: internalAction?.trigger();
        }
    }
}
