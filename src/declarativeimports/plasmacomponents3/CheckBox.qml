/*
    SPDX-FileCopyrightText: 2016 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

import QtQuick 2.6
import QtQuick.Layouts 1.3
import QtQuick.Templates @QQC2_VERSION@ as T
import QtQuick.Controls @QQC2_VERSION@
import org.kde.plasma.core 2.0 as PlasmaCore

T.CheckBox {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding,
                            implicitIndicatorWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding)
    baselineOffset: contentItem.y + contentItem.baselineOffset

    leftPadding: control.indicator && !control.mirrored ? control.indicator.width + control.spacing : 0
    rightPadding: control.indicator && control.mirrored ? control.indicator.width + control.spacing : 0
    spacing: PlasmaCore.Units.smallSpacing

    hoverEnabled: true

    indicator: CheckIndicator {
        LayoutMirroring.enabled: control.mirrored
        LayoutMirroring.childrenInherit: true
        anchors {
            left: parent.left
            verticalCenter: parent.verticalCenter
        }
        control: control
    }

    contentItem: RowLayout {
        opacity: control.enabled ? 1 : 0.6
        spacing: PlasmaCore.Units.smallSpacing

        PlasmaCore.IconItem {
            source: control.icon.name || control.icon.source
            visible: source.length > 0

            implicitWidth: PlasmaCore.Units.iconSizes.smallMedium
            implicitHeight: PlasmaCore.Units.iconSizes.smallMedium
        }

        Label {
            Layout.fillWidth: true
            text: control.text
            font: control.font
            color: PlasmaCore.ColorScope.textColor
            elide: Text.ElideRight
            visible: control.text
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }
    }
}
