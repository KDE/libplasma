/*
    SPDX-FileCopyrightText: 2012 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

import QtQuick
import org.kde.plasma.components

/*!
  \qmltype DescriptiveLabel
  \inqmlmodule org.kde.plasma.extras

  \brief This is a descriptive label which uses the plasma theme.

  The characteristics of
  the text will be automatically set according to the plasma theme. Use this
  components for less important additional data to show in a user interface.

  Example usage:
  \qml
  import org.kde.plasma.extras as PlasmaExtras
  [...]
  Column {
      PlasmaComponents.Label { text: "Firefox" }
      PlasmaExtras.DescriptiveLabel { text: "Web Browser"}
    [...]
  }
  \endqml

  See PlasmaComponents Label and primitive QML Text element API for additional
  properties, methods and signals.
 */
Label {
    id: root

    /*!
     * If a user can interact with this item, for example in a ListView delegate, this
     * property should be set to true when the label is being interacted with.
     *
     * The default is false.
     */
    property bool active: false

    opacity: active ? 0.85 : 0.75

}
