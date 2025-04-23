/*
    SPDX-FileCopyrightText: 2011 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

import QtQuick
import org.kde.kirigami as Kirigami

SequentialAnimation {
    id: pressedAnimation
    objectName: "pressedAnimation"

    property Item targetItem
    property int duration: Kirigami.Units.shortDuration

    // Fast scaling while we're animation == more FPS
    ScriptAction { script: pressedAnimation.targetItem.smooth = false }

    ParallelAnimation {
        PropertyAnimation {
            target: pressedAnimation.targetItem
            properties: "opacity"
            from: 1.0; to: 0.8
            duration: pressedAnimation.duration;
            easing.type: Easing.OutExpo;
        }
        PropertyAnimation {
            target: pressedAnimation.targetItem
            properties: "scale"
            from: 1.0; to: 0.95
            duration: pressedAnimation.duration;
            easing.type: Easing.OutExpo;
        }
    }
    ScriptAction { script: pressedAnimation.targetItem.smooth = true }
}
