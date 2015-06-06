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
    property string feedTitle
    property int maxWords: 20
    property int maxChars: 200
    property bool cached: false
    property bool broadcast
    property bool liked
    property bool fresh
    property bool expanded: false
    property int index
    property bool last: false
    property bool daterow: false

    property bool showMarkedAsRead: true

    property bool hidden: read>0 && readlater==0
    property bool showIcon: settings.viewMode==1 || settings.viewMode==3 || settings.viewMode==4 || settings.viewMode==5 ? true : false

    signal markedAsRead
    signal markedAsUnread
    signal markedReadlater
    signal unmarkedReadlater
    signal markedBroadcast
    signal unmarkedBroadcast

    enabled: !last && !daterow

    function getUrlbyUrl(url){return cache.getUrlbyUrl(url)}

    menu: last ? null : contextMenu
    contentHeight: last ?
                       app.orientation==Orientation.Portrait ? app.panelHeightPortrait : app.panelHeightLandscape :
                       daterow ? dateRowbox.height :
                       box.height + expander.height

    onHiddenChanged: {
        if (hidden && expanded) {
            expanded = false;
        }
    }

    Rectangle {
        id: background
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: Theme.rgba(Theme.highlightColor, 0.0) }
            GradientStop { position: 1.0; color: Theme.rgba(Theme.highlightColor, 0.2) }
        }
        visible: !last && !daterow
    }

    Rectangle {
        anchors.top: parent.top; anchors.right: parent.right
        width: Theme.paddingSmall; height: titleLabel.height
        visible: root.fresh && !last && !daterow
        //radius: 10

        gradient: Gradient {
            GradientStop { position: 0.0; color: Theme.rgba(Theme.highlightColor, 0.4) }
            GradientStop { position: 1.0; color: Theme.rgba(Theme.highlightColor, 0.0) }
        }
    }

    BackgroundItem {
        id: star
        anchors.right: background.right; anchors.top: background.top
        height: Theme.iconSizeSmall+2*Theme.paddingMedium
        width: height
        visible: !last && !daterow

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

    /*BackgroundItem {
        id: like
        anchors.right: background.right; anchors.top: star.bottom
        height: Theme.iconSizeSmall+2*Theme.paddingMedium
        width: height
        visible: root.liked

        Image {
            anchors.centerIn: parent;
            width: Theme.iconSizeSmall
            height: Theme.iconSizeSmall
            source: root.down ? "image://theme/icon-m-like?"+Theme.highlightColor :
                                "image://theme/icon-m-like?"+Theme.primaryColor
        }
    }

    BackgroundItem {
        id: broadcast
        anchors.right: background.right; anchors.top: star.bottom
        height: Theme.iconSizeSmall+2*Theme.paddingMedium
        width: height
        visible: root.broadcast

        Image {
            anchors.centerIn: parent;
            width: Theme.iconSizeSmall
            height: Theme.iconSizeSmall
            source: root.down ? "image://theme/icon-m-share?"+Theme.highlightColor :
                                "image://theme/icon-m-share?"+Theme.primaryColor
        }
    }*/

    Item {
        id: dateRowbox
        visible: daterow
        height: dateRowLabel.height + 2*Theme.paddingMedium
        width: parent.width
        //color: Theme.highlightColor

        Label {
            id: dateRowLabel
            anchors.right: parent.right; anchors.rightMargin: Theme.paddingMedium
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: Theme.fontSizeSmall
            font.family: Theme.fontFamilyHeading
            truncationMode: TruncationMode.Fade
            text: title
            //color: Theme.highlightDimmerColor
            color: Theme.highlightColor
        }
    }

    Item {
        id: box

        property int sizeHidden: Theme.paddingMedium + titleItem.height
        property int sizeNormal: sizeHidden + (contentLabel.visible ? Math.min(Theme.itemSizeLarge,contentLabel.height) : 0) +
                                 (entryImage.enabled ? entryImage.height + contentItem.spacing : 0) +
                                 (contentLabel.visible ? contentItem.spacing : 0)
        property int sizeExpanded: sizeHidden + (contentLabel.visible ? contentLabel.height : 0) +
                                   (entryImage.enabled ? entryImage.height + contentItem.spacing : 0) +
                                   (contentLabel.visible ? contentItem.spacing : 0)
        property bool expandable: root.hidden ? sizeHidden<sizeExpanded : sizeNormal<sizeExpanded

        height: root.expanded ? sizeExpanded : root.hidden ? sizeHidden : sizeNormal

        clip: true
        anchors.left: parent.left; anchors.right: parent.right;

        Behavior on height {
            NumberAnimation { duration: 200; easing.type: Easing.InOutQuad }
        }

        visible: !last && !daterow

        Column {
            id: contentItem
            spacing: entryImage.enabled ? Theme.paddingMedium : Theme.paddingSmall
            anchors.top: parent.top; anchors.topMargin: Theme.paddingMedium
            anchors.left: parent.left; anchors.right: parent.right

            Item {
                id: titleItem
                anchors.left: parent.left; anchors.right: parent.right;
                //anchors.leftMargin: Theme.paddingLarge
                anchors.rightMargin: star.width
                height: Math.max(titleLabel.height, icon.height+Theme.paddingSmall, iconPlaceholder.height+Theme.paddingSmall)
                //color: "red"

                // Title

                Label {
                    id: titleLabel
                    //anchors.right: parent.right; anchors.left: icon.visible ? icon.right : parent.left;
                    anchors.right: parent.right; anchors.left: icon.visible ? icon.right : iconPlaceholder.visible ? iconPlaceholder.right : parent.left
                    //anchors.leftMargin: icon.visible ? Theme.paddingMedium : Theme.paddingLarge
                    anchors.leftMargin: showIcon ? Theme.paddingMedium : Theme.paddingLarge
                    font.pixelSize: Theme.fontSizeMedium
                    font.family: Theme.fontFamilyHeading
                    font.bold: !root.read || root.readlater
                    truncationMode: TruncationMode.Fade
                    textFormat: Text.StyledText
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

                Rectangle {
                    id: iconPlaceholder
                    width: visible ? 1.2*Theme.iconSizeSmall : 0
                    height: width
                    anchors.left: parent.left;
                    anchors.top: titleLabel.top; anchors.topMargin: Theme.paddingSmall
                    y: Theme.paddingMedium
                    visible: !icon.visible && root.showIcon

                    Label {
                        id: iconPlaceholderLabel
                        anchors.centerIn: parent
                        text: feedTitle.substring(0,1).toUpperCase()
                    }

                    Component.onCompleted: {
                        //console.log("showIcon", showIcon, "icon.visible", icon.visible);
                        var r = feedTitle.length>0 ? (Math.abs(feedTitle.charCodeAt(0)-65)/57)%1 : 1;
                        var g = feedTitle.length>1 ? (Math.abs(feedTitle.charCodeAt(1)-65)/57)%1 : 1;
                        var b = feedTitle.length>2 ? (Math.abs(feedTitle.charCodeAt(2)-65)/57)%1 : 1;
                        iconPlaceholder.color = Qt.rgba(r,g,b,0.9);
                        iconPlaceholderLabel.color = (r+g+b)>1.5 ? Theme.highlightDimmerColor : Theme.primaryColor;
                    }
                }

                Rectangle {
                    anchors.fill: icon
                    //color: Theme.secondaryColor
                    //opacity: 0.3
                    color: "white"
                    visible: icon.visible
                }

                Image {
                    id: icon
                    width: visible ? 1.2*Theme.iconSizeSmall: 0
                    height: width
                    anchors.left: parent.left;
                    anchors.top: titleLabel.top; anchors.topMargin: Theme.paddingSmall
                    visible: status!=Image.Error && status!=Image.Null && root.showIcon
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

            Image {
                id: entryImage
                anchors.left: parent.left;
                fillMode: Image.PreserveAspectFit
                width: sourceSize.width>=root.width ? root.width : sourceSize.width
                enabled: source!="" && status==Image.Ready &&
                         settings.showTabIcons &&
                         sourceSize.width > Theme.iconSizeMedium &&
                         sourceSize.height > Theme.iconSizeMedium
                visible: opacity>0
                opacity: enabled ? 1.0 : 0.0
                Behavior on opacity { FadeAnimation {} }
                source: {
                    if (settings.showTabIcons && root.image!="") {
                        return settings.offlineMode ? getUrlbyUrl(root.image) : dm.online ? root.image : getUrlbyUrl(root.image);
                    } else {
                        return "";
                    }
                }
            }

            Label {
                id: contentLabel
                anchors.left: parent.left; anchors.right: parent.right;
                anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
                text: root.content
                wrapMode: Text.Wrap
                textFormat: Text.PlainText
                font.pixelSize: Theme.fontSizeSmall
                visible: root.content!=""
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
    }

    Item {
        x: box.x
        y: box.y
        OpacityRampEffect {
            sourceItem: box
            enabled: !root.expanded && box.expandable
            direction: OpacityRamp.TopToBottom
            slope: 2.0
            offset: 5 / 7
            width: box.width
            height: box.height
            anchors.fill: null
        }
    }

    BackgroundItem {
        id: expander

        visible: !last && !daterow

        anchors {
            right: parent.right
            left: parent.left
            bottom: parent.bottom
        }

        height: Math.max(expanderIcon.height,expanderLabel.height)+Theme.paddingMedium

        Image {
            id: expanderIcon
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: Theme.paddingMedium
            source: "image://theme/icon-lock-more"
            visible: box.expandable
        }

        Column {
            id: expanderLabel
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left; anchors.right: expanderIcon.left;
            anchors.leftMargin: Theme.paddingLarge
            anchors.rightMargin: Theme.paddingMedium

            Row {
                anchors.left: parent.left
                visible: !root.hidden || root.expanded
                spacing: Theme.paddingMedium

                Image {
                    width: Theme.iconSizeSmall
                    height: Theme.iconSizeSmall
                    visible: root.liked
                    source: root.down ? "image://theme/icon-m-like?"+Theme.secondaryHighlightColor :
                                        "image://theme/icon-m-like?"+Theme.secondaryColor
                }

                Image {
                    width: Theme.iconSizeSmall
                    height: Theme.iconSizeSmall
                    visible: root.broadcast
                    source: root.down ? "image://theme/icon-m-share?"+Theme.secondaryHighlightColor :
                                        "image://theme/icon-m-share?"+Theme.secondaryColor
                }
            }

            Label {
                anchors.left: parent.left; anchors.right: parent.right
                font.pixelSize: Theme.fontSizeExtraSmall
                color: root.down ? Theme.secondaryHighlightColor : Theme.secondaryColor
                truncationMode: TruncationMode.Fade
                text: root.author!=""
                      ? utils.getHumanFriendlyTimeString(date)+" • "+root.author
                      : utils.getHumanFriendlyTimeString(date)

            }
        }

        onClicked: {
            if (box.expandable) {
                root.expanded = !root.expanded
            }
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
                text: settings.signinType<10 ? readlater ? qsTr("Unsave") : qsTr("Save") :
                                                    readlater ? qsTr("Unstar") : qsTr("Star")
                onClicked: {
                    if (readlater) {
                        root.unmarkedReadlater();
                    } else {
                        root.markedReadlater();
                    }
                }
            }

            MenuItem {
                text: broadcast ? qsTr("Unshare") : qsTr("Share")
                enabled: settings.signinType >= 10
                onClicked: {
                    if (broadcast) {
                        root.unmarkedBroadcast();
                    } else {
                        root.markedBroadcast();
                    }
                }
            }

            MenuItem {
                visible: box.expandable
                text: root.expanded ? qsTr("Collapse") : qsTr("Expand")
                onClicked: {
                    root.expanded = !root.expanded;
                }
            }
        }
    }
}
