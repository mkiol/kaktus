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
    property int date
    property int read: 0
    property int readlater: 0
    property string content
    property int maxWords: 20
    property bool cached: false
    property bool expanded: false
    property int index

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
                root.readlater=0;
                entryModel.setData(index, "readlater", 0);
            } else {
                root.readlater=1;
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
            visible: isShortEqualsFull()

            function isShortEqualsFull(){
                var words = root.content.split(" ");
                var max = Math.min(words.length, root.maxWords);
                return max === root.maxWords;
            }
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
            text: getFirstWords()
            wrapMode: Text.Wrap
            textFormat: Text.PlainText
            font.pixelSize: Theme.fontSizeSmall
            visible: root.content!="" && (root.read==0 || root.readlater>0)
            color: root.read > 0 && root.readlater==0 ? Theme.secondaryColor
                                                      : Theme.primaryColor

            function getFirstWords(){
                var words = root.content.split(" ");
                var shortText = ""; var max = Math.min(words.length, root.maxWords);
                for (var i=0; i<max;i++)
                    shortText += words[i] + " ";
                return shortText;
            }
        }

        Label {
            id: fullContent
            anchors.left: parent.left; anchors.right: parent.right;
            anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
            visible: opacity > 0.0
            opacity: root.expanded ? 1.0 : 0.0
            Behavior on opacity { FadeAnimation {} }
            text: root.read>0 && root.readlater==0 ? root.content : getLastWords(root.content)
            wrapMode: Text.Wrap
            textFormat: Text.PlainText
            font.pixelSize: Theme.fontSizeSmall
            color: root.read > 0 && root.readlater==0 ? Theme.secondaryColor
                                                      : Theme.primaryColor

            function getLastWords(){
                var words = root.content.split(" ");
                var fullText = ""; var max = Math.min(words.length, root.maxWords);
                for (var i=max; i<words.length;i++)
                    fullText += words[i] + " ";
                return fullText;
            }
        }

        Label {
            function getHumanFriendlyTimeString(date) {
                var delta = Math.floor(Date.now()/1000-date);
                if (delta===0) {
                    return "just now";
                }
                if (delta===1) {
                    return "1 second ago";
                }
                if (delta<60) {
                    return "" + delta + " seconds ago";
                }
                if (delta>=60&&delta<120) {
                    return "1 minute ago";
                }
                if (delta<3600) {
                    return "" + Math.floor(delta/60) + " minutes ago";
                }
                if (delta>=3600&&delta<7200) {
                    return "1 hour ago";
                }
                if (delta<86400) {
                    return "" + Math.floor(delta/3600) + " hours ago";
                }
                if (delta>=86400&&delta<172800) {
                    return "yesterday";
                }
                if (delta<604800) {
                    return "" + Math.floor(delta/86400) + " days ago";
                }
                if (delta>=604800&&delta<1209600) {
                    return "1 week ago";
                }
                if (delta<2419200) {
                    return "" + Math.floor(delta/604800) + " weeks ago";
                }
                return Qt.formatDateTime(new Date(date*1000),"dddd, d MMMM yy");
            }
            anchors.left: parent.left; anchors.right: parent.right;
            anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingMedium
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.secondaryColor
            text: getHumanFriendlyTimeString(date)
        }

    }

    Component {
        id: contextMenu
        ContextMenu {
            MenuItem {
                text: readlater ? "Unstar" : "Star"
                onClicked: {
                    if (readlater) {
                        readlater=0;
                        entryModel.setData(index, "readlater", 0);
                    } else {
                        readlater=1;
                        entryModel.setData(index, "readlater", 1);
                    }
                }
            }
            MenuItem {
                text: read ? "Mark as unread" : "Mark as read"
                onClicked: {
                    if (read) {
                        read=0;
                        entryModel.setData(index, "read", 0);
                    } else {
                        read=1;
                        entryModel.setData(index, "read", 1);
                    }
                }
            }
        }
    }
}
