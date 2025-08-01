// SPDX-FileCopyrightText: 2019 Carl-Lucien Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2022 Felipe Kinoshita <kinofhek@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Controls as QQC2

import org.kde.plasma.components as PlasmaComponents3
import org.kde.plasma.extras as PlasmaExtras
import org.kde.ksvg as KSvg
import org.kde.kirigami as Kirigami

/*!
  \qmltype SearchField
  \inqmlmodule org.kde.plasma.extras

  \brief This is a standard textfield following KDE HIG. Using Ctrl+F as focus
  sequence and "Search…" as placeholder text.

  Example usage for the search field component:
  \qml
  import org.kde.plasma.extras as PlasmaExtras

  PlasmaExtras.SearchField {
      id: searchField
      onAccepted: console.log("Search text is " + searchField.text)
  }
  \endqml

  \since 5.93
 */
PlasmaExtras.ActionTextField {
    id: root

    property int _iconWidth: searchIcon.width + searchIcon.anchors.leftMargin

    // padding to accommodate search icon nicely
    leftPadding: if (root.effectiveHorizontalAlignment === TextInput.AlignRight) {
        return (_rightActionsRow.visible ? _rightActionsRow.width : 0) + (__hasBackgroundAndMargins ? background.margins.left : 0);
    } else {
        return _iconWidth + (_leftActionsRow.visible ? _leftActionsRow.width : 0) + (__hasBackgroundAndMargins ? background.margins.left : 0);
    }
    rightPadding: if (root.effectiveHorizontalAlignment === TextInput.AlignRight) {
        return _iconWidth + (_leftActionsRow.visible ? _leftActionsRow.width : 0) + (__hasBackgroundAndMargins ? background.margins.right : 0);
    } else {
        return (_rightActionsRow.visible ? _rightActionsRow.width : 0) + (__hasBackgroundAndMargins ? background.margins.right : 0);
    }

    Kirigami.Icon {
        id: searchIcon
        LayoutMirroring.enabled: root.effectiveHorizontalAlignment === TextInput.AlignRight
        anchors.left: root.left
        anchors.leftMargin: Kirigami.Units.smallSpacing * 2
        anchors.verticalCenter: root.verticalCenter
        anchors.verticalCenterOffset: Math.round((root.topPadding - root.bottomPadding) / 2)
        implicitHeight: Kirigami.Units.iconSizes.sizeForLabels
        implicitWidth: Kirigami.Units.iconSizes.sizeForLabels

        source: "search"
    }

    placeholderText: i18nd("libplasma6", "Search…")

    Accessible.name: i18nd("libplasma6", "Search")
    Accessible.searchEdit: true

    focusSequence: StandardKey.Find
    inputMethodHints: Qt.ImhNoPredictiveText

    rightActions: [
        Kirigami.Action {
            //ltr confusingly refers to the direction of the arrow in the icon, not the text direction which it should be used in
            icon.name: root.effectiveHorizontalAlignment === TextInput.AlignRight ? "edit-clear-locationbar-ltr" : "edit-clear-locationbar-rtl"
            visible: root.text.length > 0
            text: i18nd("libplasma6", "Clear search")
            onTriggered: {
                root.clear();
                root.accepted();
            }
        }
    ]
}
