/*
    SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

import QtQuick
import org.kde.kirigami as Kirigami
import org.kde.ksvg as KSvg

Kirigami.Icon {
    property bool selected: false
    property bool isMask: false
    //TODO: implement in libplasma
    property color color: "transparent"
}
