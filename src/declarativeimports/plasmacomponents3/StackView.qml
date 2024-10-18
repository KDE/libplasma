/*
    SPDX-FileCopyrightText: 2024 Marco Martin

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

import QtQuick
import QtQuick.Templates as T

import org.kde.kirigami as Kirigami

T.StackView {
    id: control

    property real __leading: mirrored ? -width / 4 : width - width / 4
    property real __trailing: mirrored ? width - width / 4 : -width / 4

    pushEnter: Transition {
        NumberAnimation {
            property: "opacity"
            from: 0
            to: 1
            duration: Kirigami.Units.longDuration
            easing.type: Easing.OutQuad
        }
        NumberAnimation {
            property: "x"
            from: __trailing
            to: 0
            duration: Kirigami.Units.longDuration
            easing.type: Easing.OutQuad
        }
    }
    pushExit: Transition {
        NumberAnimation {
            property: "opacity"
            from: 1
            to: 0
            duration: Kirigami.Units.longDuration
            easing.type: Easing.InQuad
        }
        NumberAnimation {
            property: "x"
            from: 0
            to: __leading
            duration: Kirigami.Units.longDuration
            easing.type: Easing.InQuad
        }
    }

    popEnter: Transition {
        NumberAnimation {
            property: "opacity"
            from: 0
            to: 1
            duration: Kirigami.Units.longDuration
            easing.type: Easing.OutQuad
        }
        NumberAnimation {
            property: "x"
            from: __leading
            to: 0
            duration: Kirigami.Units.longDuration
            easing.type: Easing.OutQuad
        }
    }
    popExit: Transition {
        NumberAnimation {
            property: "opacity"
            from: 1
            to: 0
            duration: Kirigami.Units.longDuration
            easing.type: Easing.InQuad
        }
        NumberAnimation {
            property: "x"
            from: 0
            to: __trailing
            duration: Kirigami.Units.longDuration
            easing.type: Easing.InQuad
        }
    }

    replaceEnter: Transition {
        NumberAnimation {
            property: "opacity"
            from: 0
            to: 1
            duration: Kirigami.Units.longDuration
            easing.type: Easing.OutQuad
        }
        NumberAnimation {
            property: "x"
            from: __trailing
            to: 0
            duration: Kirigami.Units.longDuration
            easing.type: Easing.OutQuad
        }
    }
    replaceExit: Transition {
        NumberAnimation {
            property: "opacity"
            from: 1
            to: 0
            duration: Kirigami.Units.longDuration
            easing.type: Easing.InQuad
        }
        NumberAnimation {
            property: "x"
            from: 0
            to: __leading
            duration: Kirigami.Units.longDuration
            easing.type: Easing.InQuad
        }
    }
}
