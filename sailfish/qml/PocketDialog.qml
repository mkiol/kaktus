/*
  Copyright (C) 2017 Michal Kosciesza <michal@mkiol.net>

  This file is part of Kaktus.

  Kaktus is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Kaktus is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Kaktus.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.0
import Sailfish.Silica 1.0


Dialog {
    id: root

    property bool showBar: false

    property string url
    property string title

    property var selectedTags: []

    function checkTags() {
        selectedTags = pocket._split(tags.text)
        tags.text = selectedTags.join(", ")
        tags.cursorPosition = tags.text.length
    }

    function addTag(tag) {
        tags.text += tags.text === "" ? tag : ", " + tag
        checkTags()
    }

    allowedOrientations: {
        switch (settings.allowedOrientations) {
        case 1:
            return Orientation.Portrait;
        case 2:
            return Orientation.Landscape;
        }
        return Orientation.Landscape | Orientation.Portrait;
    }

    onAccepted: {
        pocket.quickAdd(url, title, tags.text)
    }

    SilicaFlickable {
        id: flick

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }

        height: app.flickHeight

        contentHeight: content.height

        Column {
            id: content
            anchors {
                left: parent.left
                right: parent.right
            }

            spacing: Theme.paddingMedium

            DialogHeader {
                acceptText : qsTr("Add to Pocket")
            }

            TextArea {
                id: tags
                anchors.left: parent.left; anchors.right: parent.right
                inputMethodHints: Qt.ImhNoAutoUppercase
                wrapMode: TextEdit.WordWrap
                placeholderText: qsTr("Insert comma separated tags")
                label: qsTr("Tags")
                text: settings.pocketTags

                EnterKey.iconSource: "image://theme/icon-m-enter-close"
                EnterKey.onClicked: parent.focus = true

                onTextChanged: timer.restart()
                onFocusChanged: {
                    if (!focus) {
                        timer.stop()
                        root.checkTags()
                    }
                }

                Timer {
                    id: timer
                    interval: 1000
                    onTriggered: {
                        timer.stop()
                        root.checkTags()
                    }
                }
            }

            SectionHeader {
                text: qsTr("Previously used tags")
                visible:  settings.pocketTagsHistory !== ""
            }

            Flow {
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: Theme.horizontalPageMargin
                    rightMargin: Theme.horizontalPageMargin
                }
                spacing: Theme.paddingMedium

                Repeater {
                    model: settings.pocketTagsHistory.split(",")

                    Item {
                        width: tagLabel.width + 2 * Theme.paddingSmall
                        height: tagLabel.height
                        enabled: root.selectedTags.indexOf(modelData) ===-1

                        Label {
                            id: tagLabel
                            anchors.centerIn: parent
                            text: modelData
                            color: parent.enabled ?
                                       mouse.pressed ? Theme.highlightColor : Theme.primaryColor :
                                   Theme.secondaryColor
                        }

                        MouseArea {
                            id: mouse
                            anchors.fill: parent
                            onClicked: {
                                root.addTag(modelData)
                            }
                        }
                    }
                }
            }
        }
    }

    VerticalScrollDecorator {
        flickable: flick
    }
}
