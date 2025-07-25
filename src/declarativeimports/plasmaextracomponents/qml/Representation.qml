/*
    SPDX-FileCopyrightText: 2020 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

import QtQuick
import QtQuick.Layouts

import org.kde.plasma.components as PlasmaComponents

/*!
  \qmltype Representation
  \inqmlmodule org.kde.plasma.extras

  \brief Item to be used as root item for representations (full and compact) of plasmoids.

  It's a QtQuickControls2 Page, and like that one, has an header, a contentItem and a Footer
  It may go over the plasmoid edges (both on desktop and popups) with the properties applyHorizontalPadding and applyVerticalPadding.
  When the contentItem is a ScrollView or a Scrollarea, the plasmoid margins will be automatically removed.

  This code will create a full representation with a listview that will automatically
  fill the whole area without margins from the plasmoid popup borders

  \qml
  Plasmoid.Representation: PlasmaExtras.Representation {
      header: PlasmaExtras.BasicPlasmoidHeading {}
      contentItem: PlasmaComponent.ScrollView {
          ListView {
              // ...
          }
      }
  }
  \endqml

  \since 5.77
 */
PlasmaComponents.Page {
    id: control

    // TODO KF6: should become possible to set the paddings directly (which won't be negative anymore)
    /*!
      If true, the representation will remove any borders its container may have put and will be collapsed above its borders
     */
    property bool collapseMarginsHint: contentItem instanceof PlasmaComponents.ScrollView

    leftPadding: backgroundMetrics.getMargin("left")
    rightPadding: backgroundMetrics.getMargin("right")
    topPadding: header?.visible ? 0 : backgroundMetrics.getMargin("top")
    bottomPadding: footer?.visible ? 0 : backgroundMetrics.getMargin("bottom")

    BackgroundMetrics {
        id: backgroundMetrics

        function getMargin(margin: string): real {
            if (hasInset && control.collapseMarginsHint) {
                return -fixedMargins[margin] + inset[margin];
            } else {
                return 0;
            }
        }
    }
}
