/*
    SPDX-FileCopyrightText: 2020 Nate Graham <nate@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

import QtQuick 2.14
import QtQuick.Layouts 1.1
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents // for Highlight
import org.kde.plasma.components 3.0 as PlasmaComponents3
import org.kde.plasma.extras 2.0 as PlasmaExtras

/**
 * A list item that expands when clicked to show additional actions or a custom
 * view. The list item has a standardized appearance, with an icon on the left
 * badged with an optional emblem, a title and optional subtitle to the right,
 * an  optional default action button, and a button to expand and collapse the
 * list item.
 *
 * When expanded, the list item shows one of two views:
 * - A list of contextually-appropriate actions if contextualActionsModel has
 *   been defined and customExpandedViewContent has not been defined.
 * - A custom view if customExpandedViewContent has been defined and
 *   contextualActionsModel has not been defined.
 *
 * It is not valid to define both or neither; only define one.
 *
 * Note: this component should only be used for lists where the maximum number
 * of items is very low, ideally less than 10. For longer lists, consider using
 * a different paradigm.
 *
 *
 * Example usage:
 *
 * @code
 * import org.kde.plasma.extras 2.0 as PlasmaExtras
 * [...]
 * PlasmaExtras.ScrollArea {
 *     ListView {
 *         anchors.fill: parent
 *         focus: true
 *         currentIndex: -1
 *         clip: true
 *         model: myModel
 *         highlight: PlasmaComponents.Highlight {}
 *         highlightMoveDuration: PlasmaCore.Units.longDuration
 *         highlightResizeDuration: PlasmaCore.Units.longDuration
 *         delegate: PlasmaExtras.ExpandableListItem {
 *             icon: model.iconName
 *             iconEmblem: model.isPaused ? "emblem-pause" : ""
 *             title: model.name
 *             subtitle: model.subtitle
 *             isDefault: model.isDefault
 *             defaultActionButtonAction: Action {
 *                 icon.name: model.isPaused ? "media-playback-start" : "media-playback-pause"
 *                 text: model.isPaused ? "Resume" : "Pause"
 *                 onTriggered: {
 *                     if (model.isPaused) {
 *                         model.resume(model.name);
 *                     } else {
 *                         model.pause(model.name);
 *                     }
 *                 }
 *             }
 *             contextualActionsModel: [
 *                 Action {
 *                     icon.name: "configure"
 *                     text: "Configure..."
 *                     onTriggered: model.configure(model.name);
 *                 }
 *             ]
 *         }
 *     }
 * }
 * [...]
 * @endcode
 */
Item {
    id: listItem

    /*
     * icon: string
     * The name of the icon used in the list item.
     *
     * Required.
     */
    property alias icon: listItemIcon.source

    /*
     * iconUsesPlasmaSVG: bool
     * Whether to load the icon from the Plasma theme instead of the icon theme.
     *
     * Optional, defaults to false.
     */
    property bool iconUsesPlasmaSVG: false

    /*
     * iconEmblem: string
     * The name of the emblem to badge the icon with.
     *
     * Optional, defaults to nothing, in which case there is no emblem.
     */
    property alias iconEmblem: iconEmblem.source

    /*
     * title: string
     * The name or title for this list item.
     *
     * Optional; if not defined, there will be no title and the subtitle will be
     * vertically centered in the list item.
     */
    property alias title: listItemTitle.text

    /*
     * subtitle: string
     * The subtitle for this list item, displayed under the title.
     *
     * Optional; if not defined, there will be no subtitle and the title will be
     * vertically centered in the list item.
     */
    property alias subtitle: listItemSubtitle.text

    /*
     * subtitleCanWrap: bool
     * Whether to allow the subtitle to become a multi-line string instead of
     * eliding when the text is very long.
     *
     * Optional, defaults to false.
     */
    property bool subtitleCanWrap: false

    /*
     * subtitleColor: color
     * The color of the subtitle text
     *
     * Optional; if not defined, the subtitle will use the default text color
     */
    property alias subtitleColor: listItemSubtitle.color

    /*
     * allowStyledText: bool
     * Whether to allow the title, subtitle, and tooltip to contain styled text.
     * For performance and security reasons, keep this off unless needed.
     *
     * Optional, defaults to false.
     */
    property bool allowStyledText: false

    /*
     * defaultActionButtonAction: Action
     * The Action to execute when the default button is clicked.
     *
     * Optional; if not defined, no default action button will be displayed.
     */
    property alias defaultActionButtonAction: defaultActionButton.action

    /*
     * defaultActionButtonVisible: bool
     * When/whether to show to default action button. Useful for making it
     * conditionally appear or disappear.
     *
     * Optional; defaults to true
     */
    property bool defaultActionButtonVisible: true

    /*
     * showDefaultActionButtonWhenBusy : bool
     * Whether to continue showing the default action button while the busy
     * indicator is visible. Useful for cancelable actions that could take a few
     * seconds and show a busy indicator while processing.
     *
     * Optional; defaults to false
     */
    property bool showDefaultActionButtonWhenBusy: false

    /*
     * contextualActionsModel: list<QtObject>
     * A list of Action objects that describes additional actions that can be
     * performed on this list item. The actions should define appropriate
     * "text:", icon.name:", and "onTriggered:" properties. For example:
     *
     * @code
     * contextualActionsModel: [
     *     Action {
     *         text: "Do something"
     *         icon.name: "document-edit"
     *         onTriggered: doSomething()
     *     }
     *     Action {
     *         text: "Do something else"
     *         icon.name: "draw-polygon"
     *         onTriggered: doSomethingElse()
     *     }
     *     Action {
     *         text: "Do something completely different"
     *         icon.name: "games-highscores"
     *         onTriggered: doSomethingCompletelyDifferent()
     *     }
     * ]
     * @endcode
     *
     * Optional; if not defined, no contextual actions will be displayed and
     * you should instead assign a custom view to customExpandedViewContent,
     * which will be shown when the user expands the list item.
     */
    property list<QtObject> contextualActionsModel

    /*
     * menu: PlasmaComponents.Menu
     * The context menu to show when the user right-clicks on this list item.
     * For example:
     *
     * @code
     * contextMenu: PlasmaComponents.Menu {
     *     PlasmaComponents.MenuItem {
     *         text: "Do something"
     *         icon: "document-edit"
     *         onClicked: doSomething()
     *     }
     *     PlasmaComponents.MenuItem {
     *         text: "Do something else"
     *         icon: "draw-polygon"
     *         onClicked: doSomethingElse()
     *     }
     *     PlasmaComponents.MenuItem {
     *         text: "Do something completely different"
     *         icon: "games-highscores"
     *         onClicked: doSomethingCompletelyDifferent()
     *     }
     * }
     * @endcode
     *
     * Optional; if not defined, no context menu will be displayed when the user
     * right-clicks on the list item.
     */
    property var contextMenu

    /*
     * A custom view to display when the user expands the list item.
     *
     * This component must define width: and height: values. Width: should be
     * equal to the width of the list item itself, while height: will depend
     * on the component itself.
     *
     * Optional; if not defined, the expanded view will show contextual actions
     * instead.
     */
    property var customExpandedViewContent: actionsListComponent

    /*
     * The actual instance of the custom view content, if loaded
     * @since 5.72
     */
    property alias customExpandedViewContentItem: expandedView.item

    /*
     * isBusy: bool
     * Whether or not to display a busy indicator on the list item. Set to true
     * while the item should be non-interactive because things are processing.
     *
     * Optional; defaults to false.
     */
    property bool isBusy: false

    /*
     * isEnabled: bool
     * Whether or not this list item should be enabled and interactive.
     *
     * Optional; defaults to true.
     */
    property bool isEnabled: true

    /*
     * isDefault: bool
     * Whether or not this list item should be considered the "default" or
     * "Current" item in the list. When set to true, and the list itself has
     * more than one item in it, the list item's title and subtitle will be
     * drawn in a bold style.
     *
     * Optional; defaults to false.
     */
    property bool isDefault: false

    /*
     * expand()
     * Show the expanded view, growing the list item to its taller size.
     */
    function expand() {
        expandedView.active = true
        listItem.itemExpanded(listItem)
    }

    /*
     * collapse()
     * Hide the expanded view and collapse the list item to its shorter size.
     */
    function collapse() {
        expandedView.active = false
        listItem.itemCollapsed(listItem)
    }

    /*
     * toggleExpanded()
     * Expand or collapse the list item depending on its current state.
     */
    function toggleExpanded() {
        expandedView.active ? listItem.collapse() : listItem.expand()
    }

    signal itemExpanded(variant item)
    signal itemCollapsed(variant item)

    width: parent.width // Assume that we will be used as a delegate, not placed in a layout
    height: mainLayout.height

    onIsEnabledChanged: if (!listItem.isEnabled) { collapse() }

    // Handle left clicks and taps
    TapHandler {
        enabled: listItem.isEnabled

        acceptedButtons: Qt.LeftButton

        onSingleTapped: {
            listItem.ListView.view.currentIndex = index
            listItem.toggleExpanded()
        }
    }

    // We still need a MouseArea to handle mouse hover and right-click
    MouseArea {
        id: clickAndHoverHandler

        anchors.fill: parent

        enabled: listItem.isEnabled

        acceptedButtons: Qt.RightButton
        hoverEnabled: true

        onContainsMouseChanged: listItem.ListView.view.currentIndex = (containsMouse ? index : -1)

        // Handle right-click, if so defined
        onClicked: {
            if (contextMenu != undefined) {
                contextMenu.visualParent = parent
                contextMenu.prepare();
                contextMenu.open(mouse.x, mouse.y)
                return
            }
        }

        ColumnLayout {
            id: mainLayout

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right

            spacing: 0

            RowLayout {
                id: mainRowLayout

                Layout.fillWidth: true
                Layout.margins: PlasmaCore.Units.smallSpacing
                // Otherwise it becomes taller when the button appears
                Layout.minimumHeight: defaultActionButton.height

                // Icon and optional emblem
                PlasmaCore.IconItem {
                    id: listItemIcon

                    usesPlasmaTheme: listItem.iconUsesPlasmaSVG

                    implicitWidth: PlasmaCore.Units.iconSizes.medium
                    implicitHeight: PlasmaCore.Units.iconSizes.medium

                    PlasmaCore.IconItem {
                        id: iconEmblem

                        visible: source != undefined && source.length > 0

                        anchors.right: parent.right
                        anchors.bottom: parent.bottom

                        implicitWidth: PlasmaCore.Units.iconSizes.small
                        implicitHeight: PlasmaCore.Units.iconSizes.small
                    }
                }

                // Title and subtitle
                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignVCenter

                    spacing: 0

                    PlasmaExtras.Heading {
                        id: listItemTitle

                        visible: text.length > 0

                        Layout.fillWidth: true

                        level: 5

                        textFormat: listItem.allowStyledText ? Text.StyledText : Text.PlainText
                        elide: Text.ElideRight
                        maximumLineCount: 1

                        // Even if it's the default item, only make it bold when
                        // there's more than one item in the list, or else there's
                        // only one item and it's bold, which is a little bit weird
                        font.weight: listItem.isDefault && listItem.ListView.count > 1
                                            ? Font.Bold
                                            : Font.Normal
                    }

                    PlasmaComponents3.Label {
                        id: listItemSubtitle

                        visible: text.length > 0
                        font: PlasmaCore.Theme.smallestFont

                        // Otherwise colored text can be hard to see
                        opacity: color === PlasmaCore.Theme.textColor ? 0.7 : 1.0

                        Layout.fillWidth: true

                        textFormat: listItem.allowStyledText ? Text.StyledText : Text.PlainText
                        elide: Text.ElideRight
                        maximumLineCount: subtitleCanWrap ? 9999 : 1
                        wrapMode: subtitleCanWrap ? Text.WordWrap : Text.NoWrap
                    }
                }

                // Busy indicator
                PlasmaComponents3.BusyIndicator {
                    id: busyIndicator

                    visible: listItem.isBusy

                    // Otherwise it makes the list item taller when it appears
                    Layout.maximumHeight: defaultActionButton.implicitHeight
                    Layout.maximumWidth: Layout.maximumHeight
                }

                // Default action button
                PlasmaComponents3.Button {
                    id: defaultActionButton

                    enabled: listItem.isEnabled
                    visible: defaultActionButtonAction
                            && listItem.defaultActionButtonVisible
                            && (clickAndHoverHandler.containsMouse || expandedView.active)
                            && (!busyIndicator.visible || listItem.showDefaultActionButtonWhenBusy)
                }

                // Expand/collapse button
                PlasmaComponents3.Button {
                    visible: clickAndHoverHandler.containsMouse || expandedView.visible

                    icon.name: expandedView.active? "collapse" : "expand"

                    onClicked: listItem.toggleExpanded()
                }
            }


            // Expanded view, by default showing the actions list
            Loader {
                id: expandedView

                visible: active
                opacity: active ? 1.0 : 0

                active: false
                sourceComponent: customExpandedViewContent

                Layout.fillWidth: true
                Layout.margins: PlasmaCore.Units.smallSpacing

                Behavior on opacity {
                    NumberAnimation {
                        duration: PlasmaCore.Units.veryLongDuration
                        easing.type: Easing.InOutCubic
                    }
                }
            }
        }
    }

    // Default expanded view content: contextual actions list
    Component {
        id: actionsListComponent

        // Container for actions list, so that we can add left and right margins to it
        Item {
            height: actionsList.contentHeight
            width: mainRowLayout.width

            // TODO: Implement keyboard focus
            // TODO: Don't highlight the first item by default, unless it has focus
            // TODO: Animate the highlight moving, as in the printers applet
            ListView {
                id: actionsList

                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: listItemIcon.width + PlasmaCore.Units.smallSpacing
                anchors.rightMargin: listItemIcon.width + PlasmaCore.Units.smallSpacing * 2

                height: (PlasmaCore.Units.iconSizes.smallMedium + PlasmaCore.Units.smallSpacing * 2) * actionsList.count

                focus: true
                clip: true

                model: listItem.contextualActionsModel

                highlight: PlasmaComponents.Highlight {}

                delegate: MouseArea {
                    id: actionItem

                    enabled: model.enabled

                    width: actionsList.width
                    height: actionItemLayout.height + PlasmaCore.Units.smallSpacing * 2

                    hoverEnabled: true

                    onContainsMouseChanged: actionItem.ListView.view.currentIndex = (containsMouse ? index : -1)

                    onClicked: {
                        modelData.trigger()
                        collapse()
                    }

                    RowLayout {
                        id: actionItemLayout

                        enabled: model.enabled

                        anchors.left: parent.left
                        anchors.leftMargin: PlasmaCore.Units.smallSpacing
                        anchors.right: parent.right
                        anchors.rightMargin: PlasmaCore.Units.smallSpacing
                        anchors.verticalCenter: parent.verticalCenter

                        PlasmaCore.IconItem {
                            implicitWidth: PlasmaCore.Units.iconSizes.smallMedium
                            implicitHeight: PlasmaCore.Units.iconSizes.smallMedium

                            source: model.icon.name
                        }

                        PlasmaExtras.Heading {
                            Layout.fillWidth: true

                            level: 5

                            text: model.text
                            textFormat: listItem.allowStyledText ? Text.StyledText : Text.PlainText
                            elide: Text.ElideRight
                            maximumLineCount: 1
                        }
                    }
                }
            }
        }
    }
}
