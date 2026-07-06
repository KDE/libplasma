/*
    SPDX-FileCopyrightText: 2013 Sebastian Kügler <sebas@kde.org>
    SPDX-FileCopyrightText: 2023 ivan tkachenko <me@ratijas.tk>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.draganddrop as DragAndDrop
import org.kde.plasma.components as PlasmaComponents
import org.kde.plasma.extras as PlasmaExtras

import org.kde.kirigami as Kirigami
import org.kde.draganddrop as DragAndDrop
import org.kde.plasma.components as PlasmaComponents

PlasmaComponents.Page {
    id: root

    property bool isDragging: false

    padding: Kirigami.Units.largeSpacing

    contentItem: ColumnLayout {
        Kirigami.Heading {
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideMiddle

            level: 1
            text: "Drag & Drop"
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout {
                id: dragCol

                Layout.fillWidth: true
                Layout.fillHeight: true

                 QQC2.ItemDelegate {
                    text: "Image and URL"
                    icon.name: "image-png"
                    DragAndDrop.DragArea {
                        objectName: "imageandurl"
                        anchors { fill: parent; }
                        delegateImage: "akonadi"
                        mimeData.url: "https://plasma.kde.org/"
                        onDragStarted: root.isDragging = true
                        onDrop: root.isDragging = false
                    }
                }
                QQC2.ItemDelegate {
                    text: "HTML"
                    icon.name: "text-html"
                    DragAndDrop.DragArea {
                        objectName: "html"
                        anchors { fill: parent; }
                        mimeData.html: "<b>One <i> Two <u> Three </b> Four </i>Five </u> "
                        onDragStarted: root.isDragging = true
                        onDrop: root.isDragging = false
                    }
                }
                QQC2.ItemDelegate {
                    text: "Color"
                    icon.name: "preferences-color"
                    DragAndDrop.DragArea {
                        objectName: "color"
                        anchors { fill: parent; }
                        mimeData.color: "orange"
                        onDragStarted: root.isDragging = true
                        onDrop: root.isDragging = false
                    }
                }
                QQC2.ItemDelegate {
                    text: "Lots of Stuff"
                    icon.name: "list-add"

                    DragAndDrop.DragArea {
                        objectName: "stuff"
                        anchors.fill: parent

                        mimeData.text: "Clownfish"
                        mimeData.html: "<h2>Swimming in a Sea of Cheese</h2><pre>Primus->perform();</pre><br/>"
                        mimeData.color: "darkred"
                        mimeData.url: "https://www.kde.org/"
                        mimeData.urls: ["https://planet.kde.org", "https://fsfe.org", "https://techbase.kde.org", "https://qt.io"]

                        onDragStarted: root.isDragging = true
                        onDrop: root.isDragging = false
                    }
                }
            }

            DragAndDrop.DropArea {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: 1

                Rectangle {
                    id: clr
                    anchors.fill: parent
                    color: "transparent"
                    opacity: 0.5
                }

                PlasmaComponents.Label {
                    id: ilabel
                    text: "Drop here."
                    opacity: 1
                    anchors.centerIn: parent
                    horizontalAlignment: Text.AlignHCenter
                    PropertyAnimation { properties: "opacity"; easing.type: Easing.Linear; duration: 2000; }
                }

                onDragEnter: ilabel.text = "Drop here."
                onDragLeave: ilabel.text = "Drop exited"
                onDrop: event => {
                    var txt = event.mimeData.html;
                    txt += event.mimeData.text;
                    if (event.mimeData.url != "") {
                        txt += "<br />Url: " + event.mimeData.url;
                    }
                    var i = 0;
                    for (let u in event.mimeData.urls) {
                        txt += "<br />  Url " + i + " : " + event.mimeData.urls[i];
                        i++;
                    }
                    if (event.mimeData.hasColor()) {
                        clr.color = event.mimeData.color;
                    } else {
                        clr.color = "transparent";
                    }
                    ilabel.text = txt
                }
            }
        }
    }
}
