/****************************************************************************
**
** Copyright (C) 2011 Marco Martin  <mart@kde.org>
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
import org.kde.plasma.core 2.0 as PlasmaCore

import "." 2.0 as PlasmaComponents

/**
 * A plasma themed SelectionDialog, which can be customized by overriding the
 * ListView delegate.
 *
 * By default SelectionDialog provides a scrollable list of textual menu items.
 * The user can choose one item from the list at a time.
 */
PlasmaComponents.CommonDialog {
    id: root

    // Common API
    /**
     * type:QtObject
     * The model of selectionDialog. Can be a simple model or a custom
     * QAbstractItemModel
     */
    property alias model: listView.model

    /**
     * Selected index.
     *
     * The default value is -1.
     */
    property int selectedIndex: -1

    /**
     * The delegate used to render the items.
     */
    property Component delegate: defaultDelegate

    Component {
        id: defaultDelegate

        Label {
            visible: modelData.search(RegExp(filterField.filterText, "i")) != -1
            height: visible? paintedHeight*2 : 0
            text: modelData
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    selectedIndex = index
                    root.accept()
                }
            }

            Keys.onPressed: {
                if (event.key == Qt.Key_Up || event.key == Qt.Key_Down)
                    scrollBar.flash()
            }
        }
    }

    content: Item {
        id: contentItem
        property alias filterText: filterField.filterText
        implicitWidth: theme.mSize(theme.defaultFont).width * 40
        implicitHeight: theme.mSize(theme.defaultFont).height * 12
        height: implicitHeight

        TextField {
            id: filterField
            property string filterText
            onTextChanged: searchTimer.restart()
            clearButtonShown: true
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }
            Timer {
                id: searchTimer
                running: false
                repeat: false
                interval: 500
                onTriggered: filterField.filterText = filterField.text
            }
        }
        ListView {
            id: listView

            anchors {
                top: filterField.bottom
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            currentIndex : -1
            delegate: root.delegate
            clip: true

            Keys.onPressed: {
                if (event.key == Qt.Key_Up || event.key == Qt.Key_Down
                    || event.key == Qt.Key_Left || event.key == Qt.Key_Right
                    || event.key == Qt.Key_Select || event.key == Qt.Key_Enter
                    || event.key == Qt.Key_Return) {
                    listView.currentIndex = 0
                    event.accepted = true
                }
            }
        }

        PlasmaComponents.ScrollBar {
            id: scrollBar
            flickableItem: listView
            visible: listView.contentHeight > contentItem.height
            //platformInverted: root.platformInverted
            anchors { top: contentItem.top; right: contentItem.right }
        }
    }

    onClickedOutside: {
        reject()
    }

    Timer {
        id: focusTimer
        interval: 100
        onTriggered: {
            filterField.forceActiveFocus()
        }
    }
    onStatusChanged: {
        //FIXME: why needs focus deactivation then activation?
        if (status == DialogStatus.Open) {
            filterField.focus = false
            focusTimer.running = true
        }

        if (status == DialogStatus.Opening) {
            if (listView.currentItem != null) {
                listView.currentItem.focus = false
            }
            listView.currentIndex = -1
            listView.positionViewAtIndex(0, ListView.Beginning)
        } else if (status == DialogStatus.Open) {
            listView.focus = true
        }
    }
}
