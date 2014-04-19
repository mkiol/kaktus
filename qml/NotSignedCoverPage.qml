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

    Image {
        id: image
        source: "cover.png"
        opacity: 0.2
        anchors.top: parent.top
    }

    Label {
        anchors.centerIn: parent
        font.pixelSize: Theme.fontSizeMedium
        font.family: Theme.fontFamilyHeading
        text: qsTr("Not signed in")
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

    /*Connections {
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
    }*/
}


