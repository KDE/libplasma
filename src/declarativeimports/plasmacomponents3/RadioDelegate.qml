/*
    SPDX-FileCopyrightText: 2016 Marco Martin <mart@kde.org>
    SPDX-FileCopyrightText: 2022 ivan (@ratijas) tkachenko <me@ratijas.tk>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

import QtQuick
import QtQuick.Templates as T
import org.kde.kirigami as Kirigami
import "private" as Private

T.RadioDelegate {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding,
                            implicitIndicatorWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding)

    baselineOffset: contentItem.y + contentItem.baselineOffset
    hoverEnabled: true

    topPadding: background.margins.top
    leftPadding: background.margins.left
    rightPadding: background.margins.right
    bottomPadding: background.margins.bottom
    spacing: Kirigami.Units.smallSpacing

    icon.width: Kirigami.Units.iconSizes.sizeForLabels
    icon.height: Kirigami.Units.iconSizes.sizeForLabels

    contentItem: Private.IconLabel {
        readonly property int effectiveIndicatorWidth: control.indicator && control.indicator.visible && control.indicator.width > 0
            ? control.indicator.width + control.spacing : 0

        mirrored: control.mirrored
        leftPadding: !control.mirrored ? 0 : effectiveIndicatorWidth
        rightPadding: control.mirrored ? 0 : effectiveIndicatorWidth

        font: control.font
        alignment: Qt.AlignLeft
        display: control.display
        spacing: control.spacing
        iconItem.implicitWidth: control.icon.width
        iconItem.implicitHeight: control.icon.height
        iconItem.source: control.icon.name || control.icon.source
        label.text: control.text
    }

    indicator: RadioIndicator {
        x: control.mirrored ? control.leftPadding : control.width - width - control.rightPadding
        y: control.topPadding + Math.round((control.availableHeight - height) / 2)

        control: control
    }

    background: Private.DefaultListItemBackground {}
}
