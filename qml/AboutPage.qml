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



Page {
    id: root

    SilicaListView {
        anchors.fill: parent
        anchors.leftMargin: Theme.paddingLarge
        anchors.rightMargin: Theme.paddingLarge
        spacing: Theme.paddingLarge

        header: PageHeader {
            title: qsTr("About")
        }

        model: VisualItemModel {

            Image {
                anchors.horizontalCenter: parent.horizontalCenter
                source: "icon.png"
            }

            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: Theme.fontSizeHuge
                text: APP_NAME
            }

            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: Theme.fontSizeMedium
                color: Theme.highlightColor
                wrapMode: Text.WordWrap
                text: qsTr("Version") + ": " + VERSION;
            }

            Label {
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                anchors.left: parent.left; anchors.right: parent.right
                font.pixelSize: Theme.fontSizeSmall
                text: qsTr("Kaktus is an unofficial Netvibes client for Sailfish OS.");
            }

            Label {
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                anchors.left: parent.left; anchors.right: parent.right
                font.pixelSize: Theme.fontSizeExtraSmall
                textFormat: Text.StyledText
                text: "<a href='"+PAGE+"'>"+PAGE+"</a>";
            }

            Label {
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                anchors.left: parent.left; anchors.right: parent.right
                font.pixelSize: Theme.fontSizeExtraSmall
                textFormat: Text.RichText
                text: qsTr("Copyright &copy; 2014 Michał Kościesza");
            }

            /*Label {
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                anchors.left: parent.left; anchors.right: parent.right
                font.pixelSize: Theme.fontSizeSmall
                text: qsTr("It is free software. You can redistribute it and/or modify"
                           +" it under the terms of the GNU General Public License as published by"
                           +" the Free Software Foundation, either version 3 of the License, or"
                           +" (at your option) any later version.");
            }

            Label {
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                anchors.left: parent.left; anchors.right: parent.right
                font.pixelSize: Theme.fontSizeSmall
                text: qsTr("nReader is distributed in the hope that it will be useful,"
                           +" but WITHOUT ANY WARRANTY; without even the implied warranty of"
                           +" MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the"
                           +" GNU General Public License for more details.");
            }*/
        }

        VerticalScrollDecorator {}
    }
}
