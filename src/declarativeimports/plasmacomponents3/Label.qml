/*
    SPDX-FileCopyrightText: 2016 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

import QtQuick
import QtQuick.Templates as T
import org.kde.kirigami as Kirigami

/*!
     \qmltype Label
     \inqmlmodule org.kde.plasma.components
     \brief A simple label whose visual styling is determined by the active Plasma theme.
 */
T.Label {
    id: control

    // Work around Qt bug where left aligned text is not right aligned
    // in RTL mode unless horizontalAlignment is explicitly set.
    // https://bugreports.qt.io/browse/QTBUG-95873
    horizontalAlignment: Text.AlignLeft

    activeFocusOnTab: false

    //font data is the system one by default
    color: Kirigami.Theme.textColor
    linkColor: Kirigami.Theme.linkColor

    opacity: enabled ? 1 : 0.75

    Accessible.role: Accessible.StaticText
    Accessible.name: text
}
