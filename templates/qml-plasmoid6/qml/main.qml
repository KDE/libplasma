// SPDX-FileCopyrightText: %{CURRENT_YEAR} %{AUTHOR} <%{EMAIL}>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtQuick.Layouts
import org.kde.plasma.plasmoid
import org.kde.plasma.components as PC

PlasmoidItem {
    id: root

    fullRepresentation: ColumnLayout {
        anchors.fill: parent
        Image {
            Layout.fillHeight: true
            Layout.fillWidth: true
            fillMode: Image.PreserveAspectFit
            source: Qt.resolvedUrl(pairs.svgz)
        }
        PC.Label {
            Layout.alignment: Qt.AlignCenter
            text: "This is Plasma!"
        }
    }
}
