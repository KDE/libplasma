/*
    SPDX-FileCopyrightText: 2016 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

import QtQuick
import QtQuick.Templates as T
import org.kde.ksvg as KSvg
//NOTE: importing PlasmaCore is necessary in order to make KSvg load the current Plasma Theme
import org.kde.plasma.core as PlasmaCore
import org.kde.kirigami as Kirigami
import "private" as Private

T.Dial {
    id: control

    implicitWidth: Kirigami.Units.gridUnit * 5
    implicitHeight: implicitWidth
    hoverEnabled: true
    onPositionChanged: canvas.requestPaint()

    background:Canvas {
        id: canvas
        width: control.availableWidth
        height: control.availableHeight
        onPaint: {
            var ctx = getContext("2d");
            ctx.reset();

            var centreX = width / 2;
            var centreY = height / 2;

            ctx.globalAlpha = 0.3;
            ctx.beginPath();
            ctx.strokeStyle = control.Kirigami.Theme.textColor;
            ctx.lineWidth=5;
            ctx.arc(centreX, centreY, width/2.4, 0, 2*Math.PI, false);
            ctx.stroke();
            ctx.globalAlpha = 1;

            ctx.beginPath();
            ctx.strokeStyle = control.Kirigami.Theme.highlightColor;
            ctx.lineWidth=5;
            ctx.arc(centreX, centreY, width/2.4, 0.7*Math.PI, 1.6*Math.PI * control.position - 1.25*Math.PI, false);
            ctx.stroke();
        }
    }

    KSvg.Svg {
        id: grooveSvg
        imagePath: "widgets/slider"
        //FIXME: can this be made not necessary/less hacky?
        colorSet: control.Kirigami.Theme.colorSet
    }
    handle: Item {
        x: (control.width/2) + Math.cos((-(control.angle-90)*Math.PI)/180) * (control.width/2-width/2) - width/2
        y: (control.height/2) + Math.sin(((control.angle-90)*Math.PI)/180) * (control.height/2-height/2) - height/2

        implicitHeight: Math.floor(Kirigami.Units.gridUnit * 1.6)
        implicitWidth: implicitHeight

        Private.RoundShadow {
            id: roundShadow
            anchors.fill: parent
            state: {
                if (control.pressed) {
                    return "hidden"
                } else if (control.hovered) {
                    return "hover"
                } else if (control.activeFocus) {
                    return "focus"
                } else {
                    return "shadow"
                }
            }
        }

        KSvg.SvgItem {
            svg: KSvg.Svg {
                id: buttonSvg
                imagePath: "widgets/actionbutton"
            }
            elementId: control.pressed ? "pressed" : "normal"
            width: Math.floor(parent.height/2) * 2
            height: width
            anchors.centerIn: parent
            Behavior on opacity {
                enabled: Kirigami.Units.longDuration > 0
                NumberAnimation { duration: Kirigami.Units.longDuration }
            }
        }
    }
}
