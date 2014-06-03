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
    property alias pressed: mouseArea.pressed

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
        onClicked: {
            listItem.clicked();
        }
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

        Label {
            id: mainText
            anchors { left: parent.left; right: parent.right }
            anchors.rightMargin: star.width
            text: listItem.title
            color: mouseArea.pressed ? Theme.secondaryColor : Theme.primaryColor
            font.pixelSize: Theme.fontSizeMedium
            font.family: Theme.fontFamilyHeading
            font.bold: true
            maximumLineCount: 2
            elide: Text.ElideLeft
        }

        Label {
            id: subText
            anchors { left: parent.left; right: parent.right }
            text: listItem.content
            color: mouseArea.pressed ? Theme.secondaryColor : Theme.primaryColor
            //maximumLineCount: 2
            //elide: Text.ElideLeft
            wrapMode: Text.WordWrap
            visible: text!=""
        }

        Label {
            anchors.left: parent.left; anchors.right: parent.right;
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.secondaryColor
            elide: Text.ElideLeft
            text: root.author!=""
                  ? utils.getHumanFriendlyTimeString(date)+" • "+root.author
                  : utils.getHumanFriendlyTimeString(date)

        }
    }


}
