// -*- coding: iso-8859-1 -*-
/*
 *   Copyright 2011 Sebastian Kügler <sebas@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  2.010-1301, USA.
 */

import QtQuick 2.0
import "Animations.js" as Animations

SequentialAnimation {
    id: disappearAnimation
    objectName: "disappearAnimation"

    property Item targetItem
    property int duration: Animations.normalDuration

    ScriptAction { script: targetItem.smooth = false; }

    ParallelAnimation {
        PropertyAnimation {
            properties: "opacity"
            duration: disappearAnimation.duration
            from: 1.0
            to: 0
            target: disappearAnimation.targetItem
            easing.type: Easing.OutExpo;
        }
        PropertyAnimation {
            properties: "scale"
            target: disappearAnimation.targetItem
            from: 1.0
            to: 0.8
            duration: disappearAnimation.duration * 0.6
            easing.type: Easing.OutExpo;
        }
    }
    ScriptAction {
        script: {
            targetItem.smooth = true;
            targetItem.visible = false;
        }
    }
}
