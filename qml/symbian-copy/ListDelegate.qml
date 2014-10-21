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

    signal holded

    property alias titleText: title.text
    property string iconSource
    property int iconSize
    property bool showUnread: false
    property int unread
    property alias titleColor: title.color
    property int margins: 0
    property bool pressed: mode=="pressed"

    onPressAndHold: {
        holded();
    }

    /*onModeChanged: {
        console.log(mode)
    }*/

    subItemIndicator: true

    Item {
        anchors.fill: parent
        anchors.leftMargin: platformStyle.paddingMedium
        anchors.bottomMargin: root.margins

        property bool imageOk: root.iconSource!="" && image.status!=Image.Error && settings.showTabIcons

        Image {
            id: image
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            visible: parent.imageOk
            source: parent.imageOk ? root.iconSource : ""
            height: parent.imageOk ? platformStyle.graphicSizeSmall : 0
            width: parent.imageOk ? platformStyle.graphicSizeSmall : 0
        }

        ListItemText {
            id: title
            anchors.left: image.right
            anchors.right: unreadbox.left
            anchors.rightMargin: platformStyle.paddingSmall
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: parent.imageOk ? platformStyle.paddingMedium : 0
            mode: root.mode
            role: "Title"
            elide: Text.ElideLeft
            wrapMode: Text.Wrap
            maximumLineCount: 2
        }

        Rectangle {
            id: unreadbox
            anchors.right: parent.right
            anchors.rightMargin: 3*platformStyle.paddingLarge
            anchors.verticalCenter: parent.verticalCenter
            width: unreadlabel.width + 3 * platformStyle.paddingSmall
            height: unreadlabel.height + 2 * platformStyle.paddingSmall
            //color: listItem.pressed ? Qt.rgba(69,145,255,1) : Qt.rgba(69,145,255,1)
            //color: root.pressed ? platformStyle.colorPressed : platformStyle.colorPressed
            color: root.pressed ? platformStyle.colorPressed : Theme.highlightColor
            radius: 5
            visible: unread!==0 && showUnread

            Label {
                id: unreadlabel
                anchors.centerIn: parent
                text: unread
                color: platformStyle.colorBackground
            }
        }
    }

    /*Column {
        anchors.fill: root.padding
        Item {
            Image {

            }
            ListItemText {
                id: title
                anchors.left: image.right
                mode: root.mode
                role: "Title"
            }
        }
        ListItemText {
            visible: text !=""
            id: subtitle
            mode: root.mode
            role: "SubTitle"
        }
    }*/
}
