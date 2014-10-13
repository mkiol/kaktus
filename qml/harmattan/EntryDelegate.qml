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
    property bool hidden: read>0 && readlater==0

    property alias pressed: mouseArea.pressed
    property alias down: mouseArea.pressed
    signal clicked
    signal holded

    function getUrlbyUrl(url){return cache.getUrlbyUrl(url)}

    height: box.height + expander.height
    width: parent.width

    onHiddenChanged: {
        if (hidden && expanded) {
            expanded = false;
        }
    }

    Rectangle {
        id: background
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: Qt.rgba(255,255,255,0.05) }
            GradientStop { position: 0.5; color: Qt.rgba(0,0,0,0) }
        }
    }

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
        anchors.right: parent.right; anchors.top: parent.top
        height: Theme.iconSizeSmall+2*Theme.paddingMedium
        width: height

        platformIconId: readlater>0 ? "toolbar-favorite-mark" : "toolbar-favorite-unmark"

        onClicked: {
            if (root.readlater>0) {
                entryModel.setData(index, "readlater", 0);
            } else {
                entryModel.setData(index, "readlater", 1);
            }
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

        Column {
            id: contentItem
            spacing: Theme.paddingMedium
            anchors.top: parent.top; anchors.topMargin: Theme.paddingMedium
            anchors.left: parent.left; anchors.right: parent.right

            Item {
                id: titleItem
                anchors.left: parent.left; anchors.right: parent.right;
                anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: star.width+Theme.paddingSmall
                height: Math.max(titleLabel.height, icon.height)

                // Title

                Label {
                    id: titleLabel
                    anchors.right: parent.right; anchors.left: icon.right;
                    anchors.leftMargin: icon.visible ? Theme.paddingMedium : 0
                    font.pixelSize: Theme.fontSizeMedium
                    font.family: Theme.fontFamilyHeading
                    //font.bold: true
                    maximumLineCount: 4
                    elide: Text.ElideRight
                    wrapMode: Text.Wrap
                    text: root.title

                    color: {
                        if (hidden) {
                            if (root.down)
                                return Theme.secondaryHighlightColor;
                            return Theme.secondaryColor;
                        }

                        if (root.down)
                            return Theme.secondaryHighlightColor;
                        return Theme.highlightColor;
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

            Image {
                id: entryImage
                anchors.left: parent.left;
                anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge;
                fillMode: Image.PreserveAspectFit
                width: sourceSize.width>parent.width-2*Theme.paddingLarge ? parent.width-2*Theme.paddingLarge : sourceSize.width

                enabled: source!="" && status==Image.Ready && settings.showTabIcons &&
                         ((root.read==0 && root.readlater==0)||root.readlater>0)
                visible: opacity>0
                opacity: enabled ? 1.0 : 0.0

                source: {
                    if (settings.showTabIcons && image!="")
                        return settings.offlineMode ? getUrlbyUrl(image) : dm.online ? image : getUrlbyUrl(image);
                    else
                        return "";
                }
            }

            Label {
                id: contentLabel
                anchors.left: parent.left; anchors.right: parent.right;
                anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge

                text: root.content
                wrapMode: Text.WordWrap
                textFormat: Text.PlainText
                visible: root.content!=""

                color: {
                    if (root.hidden) {
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

    Rectangle {
        anchors.bottom: box.bottom
        width: box.width
        height: Theme.itemSizeSmall
        visible: !root.expanded && box.expandable && !root.pressed

        gradient: Gradient {
            GradientStop { position: 0.60; color: Qt.rgba(0,0,0,0) }
            GradientStop { position: 0.99; color: Qt.rgba(0,0,0,1)}
        }
    }

    Item {
        id: expander

        anchors {
            right: parent.right
            left: parent.left
            bottom: parent.bottom
        }

        height: Math.max(expanderIcon.height,expanderLabel.height)

        MouseArea {
            id: expanderMouse
            anchors.fill: parent
            onClicked: {
                if (box.expandable) {
                    root.expanded = !root.expanded
                }
            }
        }

        BorderImage {
            anchors.fill: parent
            visible: expanderMouse.pressed
            source: theme.inverted ? "image://theme/meegotouch-panel-inverted-background-pressed" : "image://theme/meegotouch-panel-background-pressed"
        }

        Image {
            id: expanderIcon
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: Theme.paddingMedium
            source: "icon-lock-more.png"
            visible: box.expandable
        }

        Label {
            id: expanderLabel
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left; anchors.right: expanderIcon.left;
            anchors.leftMargin: Theme.paddingLarge
            anchors.rightMargin: Theme.paddingMedium
            font.pixelSize: Theme.fontSizeExtraSmall
            color: root.down ? Theme.secondaryHighlightColor : Theme.secondaryColor
            elide: Text.ElideRight
            text: root.author!=""
                  ? utils.getHumanFriendlyTimeString(date)+" • "+root.author
                  : utils.getHumanFriendlyTimeString(date)

        }

    }

    FreshDash {
        visible: root.fresh>0
    }
}

