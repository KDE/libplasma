/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Components project.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
import "private/SectionScroller.js" as Sections
import org.kde.plasma.core 2.0 as PlasmaCore
import "." 2.0 as PlasmaComponents

/**
 * Similar to a ScrollBar or a ScrollDecorator.
 *
 * It's interactive and works on ListViews that have section.property set, so
 * its contents are categorized.
 *
 * An indicator will say to what category the user scrolled to. Useful for
 * things like address books or things sorted by date. Don't use with models
 * too big (thousands of items) because it implies loading all the items to
 * memory, as well loses precision.
 */
Item {
    id: root

    /**
     * The listview the sectionScroller will operate on. This component doesn't
     * work with Flickable or GridView.
     */
    property ListView listView

    onListViewChanged: {
        if (listView && listView.model)
            internal.initDirtyObserver();
    }

    Connections {
        target: listView
        onModelChanged: {
            if (listView && listView.model) {
                internal.initDirtyObserver()
            }
        }
    }

    implicitWidth: scrollBar.implicitWidth
    Behavior on opacity {
        NumberAnimation {
            duration: 250
        }
    }

    anchors {
        right: listView.right
        top: listView.top
        bottom: listView.bottom
    }


    PlasmaComponents.RangeModel {
        id: range

        minimumValue: 0
        maximumValue: Math.max(0, listView.contentHeight - listView.height)
        stepSize: 0
        //inverted: true
        positionAtMinimum: root.width*2
        positionAtMaximum: root.height - root.width*2
        value: listView.contentY
        onPositionChanged: sectionLabel.text = Sections.closestSection(position/listView.height)

    }

    PlasmaComponents.ScrollBar {
        id: scrollBar
        flickableItem: listView
        anchors.fill: parent
        interactive: true
    }
    PlasmaCore.FrameSvgItem {
        id: tooltip
        imagePath: "widgets/tooltip"
        width: sectionLabel.width + margins.left + margins.right
        height: sectionLabel.height + margins.top + margins.bottom
        Label {
            id: sectionLabel
            x: parent.margins.left
            y: parent.margins.top
        }
        y: Math.min(root.height-height-scrollBar.width, Math.max(scrollBar.width, range.position - height/2))
        anchors {
            //verticalCenter: handle.verticalCenter
            right: parent.left
        }
        opacity: sectionLabel.text && scrollBar.pressed ? 1 : 0
        Behavior on opacity {
            NumberAnimation {
                duration: 250
            }
        }
    }


    Timer {
        id: dirtyTimer
        interval: 250
        onTriggered: {
            Sections.initSectionData(listView);
            internal.modelDirty = false;
            tooltip.visible = Sections._sections.length > 1
        }
    }
    QtObject {
        id: internal

        property bool modelDirty: false
        function initDirtyObserver() {
            Sections.initSectionData(listView);
            tooltip.visible = Sections._sections.length > 1
            function dirtyObserver() {
                if (!internal.modelDirty) {
                    internal.modelDirty = true;
                    dirtyTimer.running = true;
                }
            }

            if (listView.model.countChanged)
                listView.model.countChanged.connect(dirtyObserver);

            if (listView.model.itemsChanged)
                listView.model.itemsChanged.connect(dirtyObserver);

            if (listView.model.itemsInserted)
                listView.model.itemsInserted.connect(dirtyObserver);

            if (listView.model.itemsMoved)
                listView.model.itemsMoved.connect(dirtyObserver);

            if (listView.model.itemsRemoved)
                listView.model.itemsRemoved.connect(dirtyObserver);

            sectionsRepeater.model = Sections._sections.length
        }
    }
}
