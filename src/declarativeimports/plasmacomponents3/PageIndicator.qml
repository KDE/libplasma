/*
    SPDX-FileCopyrightText: 2018 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/
pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Templates as T
import org.kde.kirigami as Kirigami

/*!
     \qmltype PageIndicator
     \inqmlmodule org.kde.plasma.components
     \brief A simple page indicator whose visual styling is determined by the active Plasma theme.
 */
T.PageIndicator {
    id: control

    /*!
        \qmlproperty PageIndicator::maximumVisibleCount

        The maximum number of dots that can be displayed at once.

        If \c count is greater than this value, only a subset of the dots
        is displayed. The subset follows the current index and uses smaller
        dots at its edges to indicate that there are hidden dots.
     */
    property int maximumVisibleCount: count

    /*!
        \qmlproperty PageIndicator::visibleCount

        The number of dots currently displayed.

        This is the smaller of \c count and \c maximumVisibleCount.
    */
    readonly property int visibleCount: Math.min(count, maximumVisibleCount)

    /*!
        \qmlproperty PageIndicator::firstVisibleIndex

        The index of the first page represented by the currently visible dots.

        When all dots are visible, this is always 0. Otherwise, the visible
        subset follows the current index while remaining within the range of
        available pages.
    */
    readonly property int firstVisibleIndex: {
        if (count <= maximumVisibleCount) {
            return 0;
        }

        const half = Math.floor(maximumVisibleCount / 2);

        return Math.min(Math.max(0, currentIndex - half), count - maximumVisibleCount);
    }

    implicitWidth: contentItem.implicitWidth + leftPadding + rightPadding
    implicitHeight: contentItem.implicitHeight + topPadding + bottomPadding

    padding: Kirigami.Units.smallSpacing
    spacing: Kirigami.Units.smallSpacing

    delegate: Rectangle {
        required property int index

        readonly property int delegateIndex: control.firstVisibleIndex + index
        readonly property bool hasPrevious: control.firstVisibleIndex > 0
        readonly property bool hasNext: control.firstVisibleIndex + control.visibleCount < control.count

        readonly property bool isLeadingIndicator: index === 0 && hasPrevious
        readonly property bool isTrailingIndicator: index === control.visibleCount - 1 && hasNext

        implicitWidth: Kirigami.Units.largeSpacing
        implicitHeight: implicitWidth

        radius: width
        color: Kirigami.Theme.textColor

        opacity: delegateIndex === control.currentIndex ? 0.9 : control.pressed ? 0.7 : 0.5

        scale: isLeadingIndicator || isTrailingIndicator ? 0.7 : 1

        Behavior on opacity {
            enabled: Kirigami.Units.longDuration > 0
            OpacityAnimator {
                duration: Kirigami.Units.longDuration
                easing.type: Easing.InOutQuad
            }
        }
    }

    contentItem: Row {
        spacing: control.spacing

        Repeater {
            model: control.visibleCount
            delegate: control.delegate
        }
    }
}
