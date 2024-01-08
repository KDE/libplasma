/*
    SPDX-FileCopyrightText: %{CURRENT_YEAR} %{AUTHOR} <%{EMAIL}>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami

ColumnLayout {
    id: root
    property alias cfg_DisplayText: textField.text

    RowLayout {
        spacing: Kirigami.Units.largeSpacing

        // To allow aligned integration in the settings form,
        // "formAlignment" is a property injected by the config containment
        // which defines the offset of the value fields
        QQC2.Label {
            Layout.minimumWidth: width
            Layout.maximumWidth: width
            width: formAlignment - Kirigami.Units.largeSpacing * 2
            horizontalAlignment: Text.AlignRight

            // use i18nd in config QML, as the default textdomain is set to that of the config container
            text: i18nd("plasma_wallpaper_org.kde.plasma.%{APPNAMELC}", "Text to Display:")
        }
        QQC2.TextField {
            id: textField
            Layout.fillWidth: true
        }
    }

    Item { // tighten layout
        Layout.fillHeight: true
    }
}
