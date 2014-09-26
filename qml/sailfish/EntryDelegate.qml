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
    property string image
    property string feedIcon
    property int maxWords: 20
    property int maxChars: 200
    property bool cached: false
    property bool fresh
    property bool expanded: false
    property int index

    property bool showMarkedAsRead: true

    signal markedAsRead
    signal markedAsUnread
    signal markedReadlater
    signal unmarkedReadlater

    function getUrlbyUrl(url){return cache.getUrlbyUrl(url)}

    menu: contextMenu
    contentHeight: item.height + 2 * Theme.paddingMedium

    Rectangle {
        id: background
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: Theme.rgba(Theme.highlightColor, 0.0) }
            GradientStop { position: 1.0; color: Theme.rgba(Theme.highlightColor, 0.2) }
        }
    }

    /*Image {
        id: background
        anchors.fill: parent
        source: "image://theme/graphic-avatar-text-back?"+Theme.highlightBackgroundColor

    }*/

    Rectangle {
        anchors.top: parent.top; anchors.left: parent.left
        width: Theme.paddingSmall; height: titleLabel.height
        visible: root.fresh
        radius: 10

        gradient: Gradient {
            GradientStop { position: 0.0; color: Theme.rgba(Theme.highlightColor, 0.4) }
            GradientStop { position: 1.0; color: Theme.rgba(Theme.highlightColor, 0.0) }
        }
    }

    /*Rectangle {
        id: background
        anchors.fill: parent
        color: Theme.rgba(Theme.highlightBackgroundColor, 0.2)
    }*/

    /*Rectangle {
        anchors.top: parent.top; anchors.bottom: parent.bottom; anchors.left: parent.left
        //color: Theme.highlightColor
        width: Theme.paddingSmall
        visible: root.fresh
        
        gradient: Gradient {
            GradientStop { position: 0.0; color: Theme.rgba(Theme.highlightColor, 0.4) }
            GradientStop { position: 0.4; color: Theme.rgba(Theme.highlightColor, 0.0) }
        }
    }*/

    /*OpacityRampEffect {
        id: effect
        slope: 1
        offset: 0.1
        //direction: fresh ? OpacityRamp.LeftToRight : OpacityRamp.BottomToTop
        direction: OpacityRamp.BottomToTop
        sourceItem: background
    }*/

    BackgroundItem {
        id: star
        anchors.right: background.right; anchors.top: background.top
        height: Theme.iconSizeSmall+2*Theme.paddingMedium
        width: height

        onClicked: {
            if (root.readlater) {
                root.unmarkedReadlater();
            } else {
                root.markedReadlater();
            }
        }

        Image {
            anchors.centerIn: parent;
            width: Theme.iconSizeSmall
            height: Theme.iconSizeSmall
            source: {
                if (root.down) {
                    if (root.readlater)
                        return "image://theme/icon-m-favorite-selected?"+Theme.highlightColor;
                    return "image://theme/icon-m-favorite?"+Theme.highlightColor;
                }
                if (root.readlater)
                    return "image://theme/icon-m-favorite-selected?"+Theme.primaryColor;
                return "image://theme/icon-m-favorite?"+Theme.primaryColor;
            }
        }
    }

    BackgroundItem {
        id: expander
        anchors.right: background.right; anchors.bottom: background.bottom
        height: visible ? lblMoreDetails.height : 0
        anchors.left: item.left
        enabled: lblMoreDetails.visible
        onClicked: {
            if (lblMoreDetails.visible)
                root.expanded = !root.expanded;
        }

        Label {
            id: lblMoreDetails
            visible: root.content.length>root.maxChars
            anchors.right: parent.right
            anchors.rightMargin: Theme.paddingMedium
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: Theme.fontSizeLarge
            text: "•••"
            color: {
                if (root.read>0 && root.readlater==0) {
                    if (root.down)
                        return Theme.secondaryHighlightColor;
                    return Theme.secondaryColor;
                }
                if (root.down)
                    return Theme.highlightColor;
                return Theme.primaryColor;
            }
        }
    }

    Column {
        id: item
        spacing: Theme.paddingMedium
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width-star.width+Theme.paddingLarge

        // Title

        Item {
            anchors.left: parent.left; anchors.right: parent.right;
            anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
            //anchors.leftMargin: Theme.paddingMedium; anchors.rightMargin: Theme.paddingMedium
            height: Math.max(titleLabel.height, icon.height)

            Label {
                id: titleLabel
                anchors.right: parent.right; anchors.left: icon.right;
                //anchors.rightMargin: Theme.paddingLarge;
                anchors.leftMargin: icon.visible ? Theme.paddingMedium : 0
                font.pixelSize: Theme.fontSizeMedium
                font.family: Theme.fontFamilyHeading
                font.bold: !root.read || root.readlater
                truncationMode: TruncationMode.Fade
                wrapMode: Text.Wrap
                text: title

                color: {
                    if (root.read>0 && root.readlater==0) {
                        if (root.down)
                            return Theme.secondaryHighlightColor;
                        return Theme.secondaryColor;
                    }
                    if (root.down)
                        return Theme.highlightColor;
                    return Theme.primaryColor;
                }
            }

            // Feed Icon

            Image {
                id: icon
                width: visible ? Theme.iconSizeSmall : 0
                height: width
                anchors.left: parent.left;
                anchors.top: titleLabel.top; anchors.topMargin: Theme.paddingSmall
                visible: status!=Image.Error && status!=Image.Null
            }

            Connections {
                target: settings
                onShowTabIconsChanged: {
                    if (root.feedIcon!="")
                        icon.source = cache.getUrlbyUrl(root.feedIcon);
                    else
                        icon.source = "";
                }
            }

            Component.onCompleted: {
                if (root.feedIcon!="") {
                    icon.source = cache.getUrlbyUrl(root.feedIcon);
                } else {
                    icon.source = "";
                }
            }
        }

        // Image

        Image {
            id: entryImage
            anchors.left: parent.left;
            anchors.leftMargin: Theme.paddingLarge;
            //anchors.leftMargin: Theme.paddingMedium;
            fillMode: Image.PreserveAspectFit
            width: sourceSize.width>root.width-2*Theme.paddingLarge ? root.width-2*Theme.paddingLarge : sourceSize.width
            //width: sourceSize.width>root.width-2*Theme.paddingMedium ? root.width-2*Theme.paddingMedium : sourceSize.width
            enabled: source!="" && status==Image.Ready && settings.showTabIcons &&
                     ((root.read==0 && root.readlater==0)||root.readlater>0)
            visible: opacity>0
            opacity: enabled ? 1.0 : 0.0
            Behavior on opacity { FadeAnimation {} }
            //Behavior on opacity {NumberAnimation { duration: 500 }}

            source: {
                if (settings.showTabIcons && image!="")
                    return settings.offlineMode ? getUrlbyUrl(image) : dm.online ? image : getUrlbyUrl(image);
                else
                    return "";
            }

        }

        // Content

        Label {
            id: shortContent
            anchors.left: parent.left; anchors.right: parent.right;
            anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
            //anchors.leftMargin: Theme.paddingMedium; anchors.rightMargin: Theme.paddingMedium
            text: {
                if (root.content.length > root.maxChars)
                    return root.content.substr(0,root.maxChars);
                return root.content;
            }

            wrapMode: Text.Wrap
            textFormat: Text.PlainText
            font.pixelSize: Theme.fontSizeSmall
            visible: root.content!="" && (root.read==0 || root.readlater>0)
            color: {
                if (root.read>0 && root.readlater==0) {
                    if (root.down)
                        return Theme.secondaryHighlightColor;
                    return Theme.secondaryColor;
                }
                if (root.down)
                    return Theme.highlightColor;
                return Theme.primaryColor;
            }
        }

        Label {
            id: fullContent
            anchors.left: parent.left; anchors.right: parent.right;
            anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
            //anchors.leftMargin: Theme.paddingMedium; anchors.rightMargin: Theme.paddingMedium
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
            color: {
                if (root.read>0 && root.readlater==0) {
                    if (root.down)
                        return Theme.secondaryHighlightColor;
                    return Theme.secondaryColor;
                }
                if (root.down)
                    return Theme.highlightColor;
                return Theme.primaryColor;
            }
        }

        Label {
            anchors.left: parent.left; anchors.right: parent.right;
            anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingMedium
            //anchors.leftMargin: Theme.paddingMedium; anchors.rightMargin: Theme.paddingMedium
            font.pixelSize: Theme.fontSizeExtraSmall
            color: root.down ? Theme.secondaryHighlightColor : Theme.secondaryColor
            truncationMode: TruncationMode.Fade
            text: root.author!=""
                  ? utils.getHumanFriendlyTimeString(date)+" • "+root.author
                  : utils.getHumanFriendlyTimeString(date)

        }

    }

    Component {
        id: contextMenu
        ContextMenu {
            MenuItem {
                text: read ? qsTr("Mark as unread") : qsTr("Mark as read")
                visible: enabled
                enabled: root.showMarkedAsRead
                onClicked: {
                    if (read) {
                        root.markedAsUnread();
                    } else {
                        root.markedAsRead();
                        if (lblMoreDetails.visible)
                            root.expanded = false;
                    }
                }
            }
            MenuItem {
                text: readlater ? qsTr("Unsave") : qsTr("Save")
                onClicked: {
                    if (readlater) {
                        root.unmarkedReadlater();
                    } else {
                        root.markedReadlater();
                    }
                }
            }
        }
    }
}
