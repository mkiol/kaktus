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

import QtQuick 2.0
import Sailfish.Silica 1.0



ListItem {
    id: root

    property string title
    property string author
    property int date
    property int read: 0
    property int readlater: 0
    property string content
    property int maxWords: 20
    property int maxChars: 200
    property bool cached: false
    property bool expanded: false
    property int index
    property int feedindex

    Component.onCompleted: console.log("index=" + index + " length=" + content.length)

    menu: contextMenu
    contentHeight: item.height + 2 * Theme.paddingMedium

    Rectangle {
        id: background
        anchors.fill: parent
        color: Theme.rgba(Theme.highlightBackgroundColor, 0.2)
    }

    OpacityRampEffect {
        id: effect
        slope: 1
        offset: 0.1
        direction: OpacityRamp.BottomToTop
        sourceItem: background
    }

    BackgroundItem {
        id: star
        anchors.right: background.right; anchors.top: background.top
        height: Theme.iconSizeSmall+2*Theme.paddingMedium
        width: height

        onClicked: {
            if (root.readlater) {
                entryModel.setData(index, "readlater", 0);
            } else {
                entryModel.setData(index, "readlater", 1);
            }
        }

        Image {
            anchors.centerIn: parent;
            width: Theme.iconSizeSmall
            height: Theme.iconSizeSmall
            source: root.readlater>0 ? "image://theme/icon-m-favorite-selected"
                                     : "image://theme/icon-m-favorite"
        }
    }

    BackgroundItem {
        id: expander
        anchors.right: background.right; anchors.bottom: background.bottom
        height: visible ? lblMoreDetails.height : 0
        anchors.left: item.left
        onClicked: {
            if (lblMoreDetails.visible)
                root.expanded = !root.expanded;
        }
        Label {
            id: lblMoreDetails
            anchors.right: parent.right
            anchors.rightMargin: Theme.paddingMedium
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: Theme.fontSizeLarge
            color: root.read>0 && root.readlater==0 ? Theme.secondaryColor
                                                    : Theme.primaryColor
            text: "•••"
            visible: root.content.length>root.maxChars
        }
    }

    Column {
        id: item
        spacing: Theme.paddingSmall
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width-star.width+Theme.paddingLarge

        // Title
        Label {
            anchors.left: parent.left; anchors.right: parent.right;
            anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
            font.pixelSize: Theme.fontSizeMedium
            font.family: Theme.fontFamilyHeading
            truncationMode: TruncationMode.Fade
            wrapMode: Text.Wrap
            text: title
            color: root.read>0 && root.readlater==0 ? Theme.secondaryColor
                                                    : Theme.highlightColor
        }

        // Content
        Label {
            id: shortContent
            anchors.left: parent.left; anchors.right: parent.right;
            anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
            text: {
                if (root.content.length > root.maxChars)
                    return root.content.substr(0,root.maxChars);
                return root.content;
            }

            wrapMode: Text.Wrap
            textFormat: Text.PlainText
            font.pixelSize: Theme.fontSizeSmall
            visible: root.content!="" && (root.read==0 || root.readlater>0)
            color: root.read > 0 && root.readlater==0 ? Theme.secondaryColor
                                                      : Theme.primaryColor
        }

        Label {
            id: fullContent
            anchors.left: parent.left; anchors.right: parent.right;
            anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
            visible: opacity > 0.0
            opacity: root.expanded ? 1.0 : 0.0
            Behavior on opacity { FadeAnimation {} }
            text : {
                if (root.read>0 && root.readlater==0)
                    return root.content;
                if (root.content.length > root.maxChars) {
                    return root.content.substr(root.maxChars);
                }
                return "";
            }

            wrapMode: Text.Wrap
            textFormat: Text.PlainText
            font.pixelSize: Theme.fontSizeSmall
            color: root.read > 0 && root.readlater==0 ? Theme.secondaryColor
                                                      : Theme.primaryColor
        }

        Label {
            anchors.left: parent.left; anchors.right: parent.right;
            anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingMedium
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.secondaryColor
            truncationMode: TruncationMode.Fade
            text: root.author!=""
                  ? app.getHumanFriendlyTimeString(date)+" | "+root.author
                  : app.getHumanFriendlyTimeString(date)
        }

    }

    Component {
        id: contextMenu
        ContextMenu {
            MenuItem {
                text: readlater ? qsTr("Unstar") : qsTr("Star")
                onClicked: {
                    if (readlater) {
                        entryModel.setData(index, "readlater", 0);
                    } else {
                        entryModel.setData(index, "readlater", 1);
                    }
                }
            }
            MenuItem {
                text: read ? qsTr("Mark as unread") : qsTr("Mark as read")
                onClicked: {
                    if (read) {
                        entryModel.setData(index, "read", 0);
                        feedModel.incrementUnread(feedindex);
                    } else {
                        entryModel.setData(index, "read", 1);
                        feedModel.decrementUnread(feedindex);
                        if (lblMoreDetails.visible)
                            root.expanded = false;
                    }
                }
            }
        }
    }
}
