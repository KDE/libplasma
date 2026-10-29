/*
 *  SPDX-FileCopyrightText: 2013 Bhushan Shah <bhush94@gmail.com>
 *  SPDX-FileCopyrightText: 2016 Kai Uwe Broulik <kde@privat.broulik.de>
 *  SPDX-FileCopyrightText: 2022 ivan tkachenko <me@ratijas.tk>
 *  SPDX-FileCopyrightText: 2023 Mike Noe <noeerover@gmail.com>
 *  SPDX-FileCopyrightText: 2023 Nate Graham <nate@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick
import Qt5Compat.GraphicalEffects

import org.kde.plasma.components as PlasmaComponents3
import org.kde.kirigami as Kirigami

/*!
  \qmltype ShadowLabel
  \inqmlmodule org.kde.plasma.extras

  \brief White text label with a black shadow behind it.

  A standardized label with white text and a black shadow behind it. When using
  software rendering such that the shadow is not available, a black rounded
  rectangle is used in its stead.

  By default it elides text on the right, wraps in a way that prefers word
  boundaries, and uses plain text formatting.

  The most important property is "text", which applies to the text property of
  the underlying Label component. See the Label component from QtQuick.Controls
  2 and primitive QML Text element API for additional properties, methods, and
  signals.

  Only the label text casts a shadow. Backgrounds and child items are
  not included.
 */
PlasmaComponents3.Label {
    id: root

    /*!
       This property can be used to conditionally *not* render the shadow, even
       when it's technically possible to render it.

       default: \c true
     */
    property bool renderShadow: true

    elide: Text.ElideRight
    wrapMode: Text.Wrap
    textFormat: Text.PlainText

    color: "white"

    // Only the shadow is drawn from a texture. Drawing the label's own glyphs
    // through a layer effect resamples them at fractional scale factors.
    Loader {
        anchors.fill: parent
        z: -1
        active: root.renderShadow && GraphicsInfo.api !== GraphicsInfo.Software
        sourceComponent: Item {
            PlasmaComponents3.Label {
                id: shadowSource
                anchors.fill: parent
                visible: false
                opacity: 1
                Accessible.ignored: true
                LayoutMirroring.enabled: false

                text: root.text
                font: root.font
                textFormat: root.textFormat
                baseUrl: root.baseUrl
                renderType: root.renderType
                renderTypeQuality: root.renderTypeQuality
                antialiasing: root.antialiasing
                smooth: root.smooth
                elide: root.elide
                wrapMode: root.wrapMode
                maximumLineCount: root.maximumLineCount
                lineHeight: root.lineHeight
                lineHeightMode: root.lineHeightMode
                fontSizeMode: root.fontSizeMode
                minimumPixelSize: root.minimumPixelSize
                minimumPointSize: root.minimumPointSize
                horizontalAlignment: root.effectiveHorizontalAlignment
                verticalAlignment: root.verticalAlignment
                topPadding: root.topPadding
                bottomPadding: root.bottomPadding
                leftPadding: root.leftPadding
                rightPadding: root.rightPadding
                style: root.style
                // Keep color alpha so the shadow matches translucent text.
                color: root.color
                styleColor: root.styleColor
                linkColor: root.linkColor
            }

            // DropShadowBase uses these private settings to draw only the shadow.
            GaussianBlur {
                x: 1
                y: 1
                width: parent.width
                height: parent.height
                source: shadowSource
                radius: 4 * Screen.devicePixelRatio
                samples: 9 * Screen.devicePixelRatio
                transparentBorder: true
                _thickness: 0.35
                _color: "black"
                _alphaOnly: true
            }
        }
    }

    // Fallback background when we can't draw the text shadow because hardware
    // rendering isn't available
    Rectangle {
        anchors {
            fill: parent
            margins: -Kirigami.Units.smallSpacing
        }

        color: "black"
        radius: Kirigami.Units.smallSpacing
        opacity: 0.45
        z: -1

        visible: GraphicsInfo.api === GraphicsInfo.Software
    }
}
