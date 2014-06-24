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
import com.nokia.symbian 1.1

import "Theme.js" as Theme

ListItem {
    id: root

    signal clicked
    signal holded
    property bool pressed: mode=="pressed"
    property bool down: mode=="pressed"
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

    onPressAndHold: {
        holded();
    }

    height: item.height + 2 * platformStyle.paddingMedium
    width: parent.width

    ToolButton {
        id: star
        anchors.horizontalCenter: mainText.horizontalCenter
        anchors.right: parent.right
        iconSource: readlater>0 ? "favourite-selected.png" : "favourite.png"

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
        anchors.left: parent.left; anchors.leftMargin: platformStyle.paddingMedium
        anchors.right: parent.right; anchors.rightMargin: platformStyle.paddingMedium
        spacing: platformStyle.paddingMedium

        // Title

        Label {
            id: mainText
            anchors { left: parent.left; right: parent.right }
            anchors.rightMargin: star.width
            text: root.title

            color: {
                if (root.read>0 && root.readlater==0) {
                    if (root.down)
                        return platformStyle.colorHighlighted;
                    return platformStyle.colorNormalMid;
                }

                if (root.down)
                    return platformStyle.colorHighlighted;
                return Theme.highlightColor;
            }

            font.pixelSize: platformStyle.fontSizeMedium
            font.bold: true
            maximumLineCount: 4
            wrapMode: Text.Wrap
            elide: Text.ElideRight
        }

        // Image

        Image {
            id: entryImage
            anchors.left: parent.left;
            visible: source!="" && status!=Image.Error && status!=Image.Null && settings.showTabIcons && ((root.read==0 && root.readlater==0)||root.readlater>0)
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
                        return platformStyle.colorHighlighted;
                    return platformStyle.colorNormalMid;
                }
                if (root.down)
                    return platformStyle.colorHighlighted;
                return platformStyle.colorNormalLight;
            }

            wrapMode: Text.WordWrap
        }


        Label {
            id: fullContent
            anchors { left: parent.left; right: parent.right }

            text : {
                if (root.read>0 && root.readlater==0)
                    return root.content;
                if (root.content.length > root.maxChars) {
                    return root.content.substr(root.maxChars);
                }
                return "";
            }

            visible: opacity > 0.0
            opacity: root.expanded ? 1.0 : 0.0
            Behavior on opacity { NumberAnimation { duration: 300 } }

            color: {
                if (root.read>0 && root.readlater==0) {
                    if (root.down)
                        return platformStyle.colorHighlighted;
                    return platformStyle.colorNormalMid;
                }
                if (root.down)
                    return platformStyle.colorHighlighted
                return platformStyle.colorNormalLight;
            }

            wrapMode: Text.WordWrap
        }

        Item {
            anchors.left: parent.left; anchors.right: parent.right;
            height: dateLabel.height + platformStyle.paddingMedium

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
                anchors.rightMargin: platformStyle.paddingLarge
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: platformStyle.fontSizeSmall
                color: root.down ? platformStyle.colorHighlighted : platformStyle.colorNormalMid
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
                    anchors.rightMargin: platformStyle.paddingMedium
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: platformStyle.fontSizeSmall
                    text: "•••"
                    visible: root.content.length>root.maxChars
                    color: {
                        if (root.read>0 && root.readlater==0) {
                            if (root.down)
                                return platformStyle.colorHighlighted
                            return platformStyle.colorNormalMid
                        }
                        if (root.down)
                            return platformStyle.colorHighlighted
                        return platformStyle.colorNormalLight
                    }
                }
            }

        }
    }


}
