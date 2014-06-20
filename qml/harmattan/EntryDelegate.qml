/*
  Copyright (C) 2014 Michal Kosciesza <michal@mkiol.net>

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

import QtQuick 1.1
import com.nokia.meego 1.0

import "Theme.js" as Theme

Item {
    id: root

    signal clicked
    signal holded
    property alias pressed: mouseArea.pressed

    property string title
    property string author
    property int date
    property int read: 0
    property int readlater: 0
    property string content
    property string image
    property int maxWords: 20
    property int maxChars: 200
    property bool cached: false
    property bool expanded: false
    property int index
    property int feedindex

    property alias down: mouseArea.pressed

    height: item.height + 2 * Theme.paddingMedium
    width: parent.width

    BorderImage {
        anchors.fill: parent
        visible: mouseArea.pressed
        source: theme.inverted ? "image://theme/meegotouch-panel-inverted-background-pressed" : "image://theme/meegotouch-panel-background-pressed"
    }

    MouseArea {
        id: mouseArea;
        anchors.fill: parent
        onClicked: root.clicked();
        onPressAndHold: root.holded();
    }


    ToolIcon {
        id: star
        anchors.horizontalCenter: mainText.horizontalCenter
        anchors.right: parent.right
        platformIconId: readlater>0 ? "toolbar-favorite-mark" : "toolbar-favorite-unmark"

        onClicked: {
            if (root.readlater>0) {
                entryModel.setData(index, "readlater", 0);
            } else {
                entryModel.setData(index, "readlater", 1);
            }
        }
    }

    Column {
        id: item
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left; anchors.leftMargin: Theme.paddingMedium
        anchors.right: parent.right; anchors.rightMargin: Theme.paddingMedium

        spacing: Theme.paddingMedium

        // Title

        Label {
            id: mainText
            anchors { left: parent.left; right: parent.right }
            anchors.rightMargin: star.width
            text: listItem.title

            color: {
                if (root.read>0 && root.readlater==0) {
                    if (root.down)
                        return Theme.secondaryHighlightColor;
                    return Theme.secondaryColor;
                }

                if (root.down)
                    return Theme.secondaryHighlightColor;
                return Theme.highlightColor;
            }

            font.pixelSize: Theme.fontSizeMedium
            font.family: Theme.fontFamilyHeading
            font.bold: true
            maximumLineCount: 2
            elide: Text.ElideRight
        }

        // Image

        Image {
            id: entryImage
            anchors.left: parent.left;
            visible: source!="" && status!=Image.Error && status!=Image.Null && settings.showTabIcons
            fillMode: Image.PreserveAspectFit
            width: sourceSize.width>parent.width ? parent.width : sourceSize.width
        }

        Connections {
            target: settings
            onShowTabIconsChanged: {
                if (settings.showTabIcons && image!="")
                    entryImage.source = settings.offlineMode ? cache.getUrlbyUrl(image) : dm.online ? image : cache.getUrlbyUrl(image);
                else
                    entryImage.source = "";
            }
        }

        Component.onCompleted: {
            if (settings.showTabIcons && image!="")
                entryImage.source = settings.offlineMode ? cache.getUrlbyUrl(image) : dm.online ? image : cache.getUrlbyUrl(image);
            else
                entryImage.source = "";
        }

        // Content

        Label {
            id: shortContent
            anchors { left: parent.left; right: parent.right }
            text: {
                if (root.content.length > root.maxChars)
                    return root.content.substr(0,root.maxChars);
                return root.content;
            }
            visible: root.content!="" && (root.read==0 || root.readlater>0)

            color: {
                if (root.read>0 && root.readlater==0) {
                    if (root.down)
                        return Theme.secondaryHighlightColor;
                    return Theme.secondaryColor;
                }
                if (root.down)
                    return Theme.secondaryColor;
                return Theme.primaryColor;
            }

            wrapMode: Text.WordWrap
        }


        Label {
            id: fullContent
            anchors { left: parent.left; right: parent.right }
            text: listItem.content

            visible: opacity > 0.0
            opacity: root.expanded ? 1.0 : 0.0
            Behavior on opacity { NumberAnimation { duration: 300 } }

            color: {
                if (root.read>0 && root.readlater==0) {
                    /*if (root.down)
                        return Theme.secondaryHighlightColor;*/
                    return Theme.secondaryColor;
                }
                if (root.down)
                    return Theme.secondaryColor
                return Theme.primaryColor;
            }

            wrapMode: Text.WordWrap
        }

        Item {
            anchors.left: parent.left; anchors.right: parent.right;
            height: dateLabel.height + Theme.paddingMedium

            /*Rectangle {
                anchors.fill: parent
                color: mouseExpander.pressed ? Qt.rgba(255,255,255,0.1) : Qt.rgba(255,255,255,0.0)
            }*/

            MouseArea {
                id: mouseExpander
                anchors.fill: parent
                onClicked: {
                    if (lblMoreDetails.visible)
                        root.expanded = !root.expanded;
                }
            }

            Label {
                id: dateLabel
                anchors.left: parent.left; anchors.right: expander.left;
                anchors.rightMargin: Theme.paddingLarge
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.secondaryColor
                elide: Text.ElideRight
                text: root.author!=""
                      ? utils.getHumanFriendlyTimeString(date)+" • "+root.author
                      : utils.getHumanFriendlyTimeString(date)
            }

            Item {
                id: expander
                anchors.right: parent.right;
                anchors.verticalCenter: parent.verticalCenter
                height: visible ? lblMoreDetails.height : 0
                width: lblMoreDetails.width
                anchors.left: root.left
                enabled: lblMoreDetails.visible

                Label {
                    id: lblMoreDetails
                    anchors.right: parent.right
                    anchors.rightMargin: Theme.paddingMedium
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: Theme.fontSizeSmall
                    text: "•••"
                    visible: root.content.length>root.maxChars
                    color: {
                        if (root.read>0 && root.readlater==0) {
                            if (root.down)
                                return Theme.secondaryHighlightColor;
                            return Theme.secondaryColor;
                        }
                        if (root.down)
                            return Theme.secondaryColor;
                        return Theme.primaryColor;
                    }
                }
            }

        }
    }


}
