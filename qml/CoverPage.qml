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

    Column {
        anchors.centerIn: parent
        visible: !dm.busy && !fetcher.busy
        spacing: Theme.paddingMedium

        Image {
            source: "icon-small.png"
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Label {
            font.pixelSize: Theme.fontSizeMedium
            font.family: Theme.fontFamilyHeading
            anchors.horizontalCenter: parent.horizontalCenter
            text: APP_NAME
        }

        Item {
            height: Theme.paddingLarge
            width: Theme.paddingLarge
        }
    }

    Column {
        anchors.centerIn: parent
        visible: dm.busy || fetcher.busy
        spacing: Theme.paddingMedium

        Image {
            source: "icon-small.png"
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Label {
            id: label
            font.pixelSize: Theme.fontSizeMedium
            font.family: Theme.fontFamilyHeading
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Label {
            id: progressLabel
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: Theme.fontSizeLarge
            font.family: Theme.fontFamilyHeading
            color: Theme.highlightColor
        }

        Item {
            height: 2*Theme.paddingLarge
            width: Theme.paddingLarge
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

        onInitiating: label.text = qsTr("Initiating")
        onUpdating: label.text = qsTr("Updating")
        onUploading: label.text = qsTr("Uploading")
        onCheckingCredentials: label.text = qsTr("Signing in")
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


