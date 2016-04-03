/*
  Copyright (C) 2016 Michal Kosciesza <michal@mkiol.net>

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

import QtQuick 2.1
import Sailfish.Silica 1.0

CoverBackground {
    id: root

    property int unread: 0
    property bool active: status === Cover.Active
    property bool busy: app.fetcherBusyStatus || dm.busy
    property string label
    property string unreadLabel: {
        if (root.unread==0)
            return qsTr("All read");
        if (root.unread==1)
            return unread + " " + qsTr("unread item");
        if (root.unread<5)
            return unread + " " + qsTr("unread items","less than 5 articles are unread");
        return unread + " " + qsTr("unread items","more or equal 5 articles are unread");
    }

    onStatusChanged: {
        if (status === Cover.Active) {
            root.unread = utils.countUnread();
        }
    }

    function connectFetcher() {
        if (typeof fetcher === 'undefined')
            return;
        fetcher.progress.connect(fetcherProgress);
        fetcher.busyChanged.connect(fetcherBusyChanged);
    }

    function disconnectFetcher() {
        if (typeof fetcher === 'undefined')
            return;
        fetcher.progress.disconnect(fetcherProgress);
        fetcher.busyChanged.disconnect(fetcherBusyChanged);
    }

    function fetcherProgress(current, total) {
        label = qsTr("Syncing");
        if (total > 0)
            label += " " + Math.floor((current/total)*100)+"%";
    }

    function fetcherUploadProgress(current, total) {
        label = qsTr("Uploading");
        if (total > 0)
            label += " " + Math.floor((current/total)*100)+"%";
    }

    function fetcherBusyChanged() {
        switch(fetcher.busyType) {
        case 1:
            label = qsTr("Initiating");
            label += " " + "0%"
            break;
        case 2:
            label = qsTr("Updating")
            label += " " + "0%"
            break;
        case 3:
        case 4:
            label = qsTr("Signing in")
            break;
        case 11:
        case 21:
        case 31:
            label = qsTr("Waiting");
            break;
        }

        if (!fetcher.busy && active) {
            root.unread = utils.countUnread();
        }
    }


    Connections {
        target: dm

        onProgress: {
            if (!app.fetcherBusyStatus) {
                label = qsTr("Caching");
                if (current > 0 && total != 0)
                    label += " " + qsTr("%1 of %2").arg(current).arg(total);
            }
        }
    }

    CoverActionList {
        enabled: !root.busy && settings.signedIn
        CoverAction {
            iconSource: "image://theme/icon-cover-sync"
            onTriggered: fetcher.update();
        }
    }

    CoverActionList {
        enabled: root.busy && settings.signedIn
        CoverAction {
            iconSource: "image://theme/icon-cover-cancel"
            onTriggered: {
                dm.cancel();
                fetcher.cancel();
            }
        }
    }

    /*BusyIndicator {
        anchors.centerIn: parent
        running: root.busy
        size: BusyIndicatorSize.Medium
    }*/

    CoverPlaceholder {
        text: settings.signedIn ? root.busy ? root.label : root.unreadLabel : qsTr("Not signed in")
        icon.source: "icon-small.png"
    }
}
