/* Copyright (C) 2016-2022 Michal Kosciesza <michal@mkiol.net>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

import QtQuick 2.1
import Sailfish.Silica 1.0

CoverBackground {
    id: root

    property int unread: 0
    property bool active: status === Cover.Active
    property bool busy: app.fetcherBusyStatus || dm.busy
    property string label
    property string unreadLabel: unread > 0 ?
                                     qsTr("%n unread item(s)", "", unread) :
                                     qsTr("All read")
    onStatusChanged: {
        if (status === Cover.Active) {
            root.unread = utils.countUnread();
        }
    }

    function connectFetcher() {
        if (typeof fetcher === 'undefined') return;
        fetcher.progress.connect(fetcherProgress);
        fetcher.busyChanged.connect(fetcherBusyChanged);
    }

    function disconnectFetcher() {
        if (typeof fetcher === 'undefined') return;
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

    CoverPlaceholder {
        text: settings.signedIn ? root.busy ? root.label : root.unreadLabel : APP_NAME
        icon.source: settings.appIcon()
    }
}
