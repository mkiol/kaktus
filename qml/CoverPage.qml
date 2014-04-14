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


CoverBackground {
    id: root

    property int unread: 0

    onStatusChanged: {
        if (status==Cover.Active) {
            root.unread = utils.getUnreadItemsCount();
        }
    }

    Image {
        id: image
        source: "icon-small.png"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top; anchors.topMargin: 2*Theme.paddingLarge
    }

    Column {
        anchors.top: parent.verticalCenter
        visible: !dm.busy && !fetcher.busy
        spacing: Theme.paddingMedium
        anchors.horizontalCenter: parent.horizontalCenter

        Rectangle {
            id: unreadbox
            anchors.horizontalCenter: parent.horizontalCenter
            width: unreadlabel.width + 3 * Theme.paddingSmall
            height: unreadlabel.height + 2 * Theme.paddingSmall
            color: Theme.rgba(Theme.highlightBackgroundColor, 0.2)
            radius: 5
            //visible: root.unread!=0

            Label {
                id: unreadlabel
                anchors.centerIn: parent
                text: root.unread!=0 ? root.unread : qsTr("all read")
                //color: listItem.down ? Theme.secondaryHighlightColor : Theme.secondaryColor
                color: Theme.highlightColor
                horizontalAlignment: Text.AlignHCenter
            }
        }

        /*Label {
            font.pixelSize: Theme.fontSizeMedium
            font.family: Theme.fontFamilyHeading
            anchors.horizontalCenter: parent.horizontalCenter
            text: APP_NAME
            visible: root.unread==0
        }*/
    }

    Column {
        anchors.top: parent.verticalCenter
        visible: dm.busy || fetcher.busy
        spacing: Theme.paddingMedium
        anchors.horizontalCenter: parent.horizontalCenter

        Label {
            id: label
            font.pixelSize: Theme.fontSizeMedium
            font.family: Theme.fontFamilyHeading
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Label {
            id: progressLabel
            anchors.horizontalCenter: parent.horizontalCenter
            //font.pixelSize: Theme.fontSizeLarge
            //font.family: Theme.fontFamilyHeading
            color: Theme.highlightColor
        }
    }

    CoverActionList {
        enabled: !dm.busy && !fetcher.busy
        CoverAction {
            iconSource: "image://theme/icon-cover-sync"
            onTriggered: fetcher.update();
        }
    }

    CoverActionList {
        enabled: dm.busy || fetcher.busy
        CoverAction {
            iconSource: "image://theme/icon-cover-cancel"
            onTriggered: {
                dm.cancel();
                fetcher.cancel();
            }
        }
    }

    Connections {
        target: fetcher

        onProgress: {
            label.text = qsTr("Syncing");
            progressLabel.text = Math.floor((current/total)*100)+"%";
        }

        onBusyChanged: {
            switch(fetcher.busyType) {
            case 1:
                label.text = qsTr("Initiating");
                progressLabel.text = "0%"
                break;
            case 2:
                label.text = qsTr("Updating")
                progressLabel.text = "0%"
                break;
            case 3:
                label.text = qsTr("Signing in")
                progressLabel.text = "";
                break;
            }

            if (!fetcher.busy)
                root.unread = utils.getUnreadItemsCount();
        }
    }

    Connections {
        target: dm

        onProgress: {
            if (!fetcher.busy) {
                label.text = qsTr("Caching");
                progressLabel.text = remaining;
            }
        }
    }
}


