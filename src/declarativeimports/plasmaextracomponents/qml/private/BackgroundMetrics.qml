/*
    SPDX-FileCopyrightText: 2020 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/
import QtQuick

import org.kde.plasma.plasmoid
import org.kde.plasma.core as PlasmaCore
import org.kde.ksvg as KSvg

KSvg.FrameSvgItem {
    id: backgroundMetrics
    visible: false
    imagePath: {
        if (Window.window instanceof PlasmaCore.AppletPopup || Window.window instanceof PlasmaCore.Dialog) {
            return "dialogs/background";
        } else if (Plasmoid.formFactor === PlasmaCore.Types.Planar) {
            return "widgets/background";
        // panels and other formfactors are explicitly not supported
        } else {
            return "";
        }
    }
    readonly property bool hasInset: backgroundMetrics.inset.left >= 0 && backgroundMetrics.inset.right >= 0 && backgroundMetrics.inset.top >= 0 && backgroundMetrics.inset.bottom >= 0
}
