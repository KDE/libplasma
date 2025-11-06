// SPDX-FileCopyrightText: 2019 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import QtQuick.Templates as T
import org.kde.plasma.components as PlasmaComponents3
import org.kde.kirigami as Kirigami

/*!
  \qmltype ActionTextField
  \inqmlmodule org.kde.plasma.extras

  \brief This is advanced textfield. It is recommended to use this class when there
  is a need to create a create a textfield with action buttons (e.g a clear
  action).

  For common pattern like, a search field or a password field, prefer using the
  more specific SearchField or PasswordField.

  Example usage for a search field:
  \qml
  import QtQuick.Controls as QQC2
  import org.kde.plasma.extras as PlasmaExtras

  PlasmaExtras.ActionTextField {
      id: searchField

      placeholderText: "Search…"

      focusSequence: StandardKey.Find

      rightActions: [
          QQC2.Action {
              icon.name: "edit-clear"
              enabled: searchField.text !== ""
              onTriggered: {
                  searchField.clear()
                  searchField.accepted()
              }
          }
      ]

      onAccepted: console.log("Search text is " + searchField.text)
  }
  \endqml

  \since 5.93
 */
PlasmaComponents3.TextField {
    id: root

    /*!
      \qmlproperty keysequence ActionTextField::focusSequence
      This property holds a shortcut sequence that will focus the text field.

      \since 5.93
    */
    property alias focusSequence: focusShortcut.sequence

    /*!
      This property holds a list of actions that will be displayed on the left side of the text field.

      By default this list is empty.

      \since 5.93
     */
    property list<T.Action> leftActions

    /*!
      This property holds a list of actions that will be displayed on the right side of the text field.

      By default this list is empty.

      \since 5.93
     */
    property list<T.Action> rightActions

    property alias _leftActionsRow: leftActionsRow
    property alias _rightActionsRow: rightActionsRow

    hoverEnabled: true

    horizontalAlignment: Qt.AlignLeft
    LayoutMirroring.enabled: Application.layoutDirection === Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    topPadding: __hasBackgroundAndMargins ? background.margins.top : 0
    bottomPadding: __hasBackgroundAndMargins ? background.margins.bottom : 0
    leftPadding: if (root.effectiveHorizontalAlignment === TextInput.AlignRight) {
        return _rightActionsRow.width + (__hasBackgroundAndMargins ? background.margins.left : 0);
    } else {
        return _leftActionsRow.width + (__hasBackgroundAndMargins ? background.margins.left : 0);
    }
    rightPadding: if (root.effectiveHorizontalAlignment === TextInput.AlignRight) {
        return _leftActionsRow.width + (__hasBackgroundAndMargins ? background.margins.right : 0);
    } else {
        return _rightActionsRow.width + (__hasBackgroundAndMargins ? background.margins.right : 0);
    }

    Behavior on leftPadding {
        enabled: Kirigami.Units.longDuration > 0
        NumberAnimation {
            duration: Kirigami.Units.longDuration
            easing.type: Easing.InOutQuad
        }
    }

    Behavior on rightPadding {
        enabled: Kirigami.Units.longDuration > 0
        NumberAnimation {
            duration: Kirigami.Units.longDuration
            easing.type: Easing.InOutQuad
        }
    }

    Shortcut {
        id: focusShortcut
        enabled: root.visible && root.enabled
        onActivated: {
            root.forceActiveFocus(Qt.ShortcutFocusReason)
            root.selectAll()
        }

        // here to make it private
        component InlineActionIcon: PlasmaComponents3.ToolButton {
            required property T.Action modelData

            icon.width: Kirigami.Units.iconSizes.sizeForLabels
            icon.height: Kirigami.Units.iconSizes.sizeForLabels

            Layout.fillHeight: true
            Layout.preferredWidth: implicitHeight

            icon.name: modelData.icon.name.length > 0 ? modelData.icon.name : modelData.icon.source
            text: modelData.text
            display: PlasmaComponents3.ToolButton.IconOnly

            visible: !(modelData instanceof Kirigami.Action) || modelData.visible
            enabled: modelData.enabled

            onClicked: mouse => modelData.trigger()

            PlasmaComponents3.ToolTip.visible: (hovered || activeFocus) && (text.length > 0)
            PlasmaComponents3.ToolTip.text: text
        }
    }

    PlasmaComponents3.ToolTip.visible: focusShortcut.nativeText.length > 0 && root.text.length === 0 && !rightActionsRow.hovered && !leftActionsRow.hovered && hovered
    PlasmaComponents3.ToolTip.text: focusShortcut.nativeText
    PlasmaComponents3.ToolTip.delay: Kirigami.Settings.tabletMode ? Qt.styleHints.mousePressAndHoldInterval : Kirigami.Units.toolTipDelay

    RowLayout {
        id: leftActionsRow

        anchors {
            margins: 1
            top: parent.top
            left: parent.left
            bottom: parent.bottom
        }

        visible: root.leftActions.length > 0

        spacing: Kirigami.Units.smallSpacing
        layoutDirection: Qt.LeftToRight

        Repeater {
            model: root.leftActions
            InlineActionIcon { }
        }
    }

    RowLayout {
        id: rightActionsRow

        anchors {
            margins: 1
            top: parent.top
            right: parent.right
            bottom: parent.bottom
        }

        visible: root.rightActions.length > 0

        spacing: Kirigami.Units.smallSpacing
        layoutDirection: Qt.RightToLeft

        Repeater {
            model: root.rightActions
            InlineActionIcon { }
        }
    }
}
