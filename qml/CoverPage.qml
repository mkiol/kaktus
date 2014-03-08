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

    property bool busy: false

    Image {
        anchors.centerIn: parent
        source: "icon.png"
        visible: !root.busy
    }

    Column {
        visible: root.busy
        anchors.left: parent.left; anchors.right: parent.right
        anchors.margins: Theme.paddingSmall
        anchors.verticalCenter: parent.verticalCenter
        spacing: Theme.paddingMedium

        Label {
            id: label
            anchors.left: parent.left; anchors.right: parent.right
            font.pixelSize: Theme.fontSizeLarge
            font.family: Theme.fontFamilyHeading
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap
        }

        /*BusyIndicator {
            anchors.horizontalCenter: parent.horizontalCenter
            size: BusyIndicatorSize.Medium
            running: root.busy
        }*/

        Label {
            id: progressLabel
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: Theme.fontSizeLarge
            font.family: Theme.fontFamilyHeading
        }
    }

    CoverActionList {
        CoverAction {
            id: action
            iconSource: root.busy ? "image://theme/icon-cover-cancel" : "image://theme/icon-cover-sync"
            onTriggered: {
                if (root.busy) {
                    dm.cancel();
                    fetcher.cancel();
                } else {
                    dm.cancel();
                    fetcher.cancel();
                    fetcher.update();
                }
            }
        }
    }

    Connections {
        target: fetcher
        onBusy: {
            label.text = qsTr("Syncing...");
            progressLabel.text = "0%";
            root.busy = true
        }
        onError: {
            root.busy = false;
            //label.text = qsTr("Error!");
        }
        onReady: {
            if (!dm.isBusy())
                root.busy = false;
        }
        onProgress: {
            label.text = qsTr("Syncing...");
            progressLabel.text = Math.floor((current/total)*100)+"%";
        }
    }

    Connections {
        target: dm
        onCanceled: {
            if (!fetcher.isBusy())
                root.busy = false;
        }
        onBusy: root.busy = true
        onReady: {
            if (!fetcher.isBusy()) {
                label.text = qsTr("Caching...");
                root.busy = false;
            }
        }
        onProgress: {
            if (!fetcher.isBusy()) {
                label.text = qsTr("Caching...");
                progressLabel.text = remaining;
            }
        }
    }
}


