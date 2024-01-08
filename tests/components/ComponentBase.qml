/*
 * SPDX-FileCopyrightText: 2019 David Edmundson <kde@davidedmundson.co.uk>
 * SPDX-FileCopyrightText: 2020 Noah Davis <noahadvs@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */
import QtQuick
import QtQuick.Layouts
import org.kde.plasma.components as PlasmaComponents
import org.kde.kirigami as Kirigami

PlasmaComponents.Page {
    id: mainPage
    title: "Test"
    width: Math.max(mainPage.contentWidth, mainPage.implicitHeaderWidth, mainPage.implicitFooterWidth)
        + leftPadding + rightPadding
    height: mainPage.contentHeight + mainPage.implicitHeaderHeight + mainPage.implicitFooterHeight
        + topPadding + bottomPadding

    padding: Kirigami.Units.gridUnit

    header: PlasmaComponents.ToolBar {
        leftPadding: mainPage.padding
        rightPadding: mainPage.padding
        contentItem: Kirigami.Heading {
            text: mainPage.title
            /* FIXME: this line is needed to prevent vertical pixel
             * misalignment of controls, such as checkboxes.
             * The cause of the problem is unknown.
             */
            level: 2
        }
    }

    background: Rectangle {
        color:  Kirigami.Theme.backgroundColor
    }
}

