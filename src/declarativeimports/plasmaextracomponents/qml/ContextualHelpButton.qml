/*
   SPDX-FileCopyrightText: 2020 Felix Ernst <fe.a.ernst@gmail.com>
   SPDX-FileCopyrightText: 2024 Nate Graham <nate@kde.org>
   SPDX-FileCopyrightText: 2024 ivan tkachenko <me@ratijas.tk>
   SPDX-FileCopyrightText: 2026 Filip Fila <filipfila.kde@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

import QtQuick
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.plasma.components as PlasmaComponents

/*!
  \qmltype ContextualHelpButton
  \inqmlmodule org.kde.plasmaextras

  \brief An inline help button that shows a tooltip when clicked, made for desktop use.

  Use this component when you want to explain details or usage of a feature of
  the UI, but the explanation is too long to fit in an inline label, and too
  important to put in a hover tooltip and risk the user missing it.

  Example usage:
  \qml
  import QtQuick.Layouts
  import org.kde.plasma.extras as PlasmaExtras

  RowLayout {
      spacing: Kirigami.Units.smallSpacing

      PlasmaComponents.CheckBox {
          text: i18ndc("your_app_name", "@option:check", "Allow screen tearing in fullscreen windows")
      }

      PlasmaExtras.ContextualHelpButton {
          toolTipText: i18ndc("your_app_name", "@info:tooltip", "With most displays, screen tearing reduces latency at the cost of some visual fidelity at high framerates. Note that not all graphics drivers support this setting.")
      }
  }

  \endqml
 */

PlasmaComponents.ToolButton {
    id: root

    /*!
      \qmlproperty string ContextualHelpButton::toolTipText
        The text to display inside the contextual help tooltip.
     */
    property alias toolTipText: toolTip.text

    /*!
     */
    property bool toolTipVisible: false

    text: i18ndc("libplasma6", "@action:button", "Show Contextual Help")
    icon.name: "help-contextual-symbolic"
    display: PlasmaComponents.ToolButton.IconOnly

    Accessible.description: toolTipText

    onReleased: {
        toolTip.delay = toolTipVisible ? Kirigami.Units.toolTipDelay : 0;
        toolTipVisible = !toolTipVisible;
    }
    onActiveFocusChanged: {
        toolTip.delay = Kirigami.Units.toolTipDelay;
        toolTipVisible = false;
    }
    Layout.maximumHeight: parent?.height ?? -1

    PlasmaComponents.ToolTip {
        id: toolTip
        clip: true
        visible: root.hovered || root.toolTipVisible || toolTipHandler.hovered
        onVisibleChanged: {
            if (!visible && root.toolTipVisible) {
                root.toolTipVisible = false;
                delay = Kirigami.Units.toolTipDelay;
            }
        }
        timeout: -1 // Don't disappear while the user might still be reading it!

        HoverHandler {
            // Also keep the tooltip open while hovering it
            // Fixes the flickering when the popup covers the button
            id: toolTipHandler
            enabled: !root.toolTipVisible // Only if activated by hovering
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.WhatsThisCursor
        acceptedButtons: Qt.NoButton
    }
}
