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


PullDownMenu {
    id: root

    property bool showAbout: true
    property bool showSettings: true
    property bool showSync: true
    property bool showMarkAsRead: true
    property bool showMarkAsUnread: true

    signal markedAsRead
    signal markedAsUnread

    MenuItem {
        text: settings.offlineMode ? qsTr("Set to: Online") : qsTr("Set to: Offline")

        onClicked: {
            settings.offlineMode = !settings.offlineMode;
        }
    }

    MenuItem {
        text: qsTr("About")
        visible: root.showAbout

        onClicked: {
            pageStack.push(Qt.resolvedUrl("AboutPage.qml"));
        }
    }

    MenuItem {
        text: qsTr("Settings")
        visible: root.showSettings

        onClicked: {
            pageStack.push(Qt.resolvedUrl("SettingsPage.qml"));
        }
    }

    MenuItem {
        text: enabled ? qsTr("Sync") : qsTr("Syncing...")
        visible: root.showSync

        onClicked: fetcher.update()
        enabled: !fetcher.busy && !dm.busy
    }



    MenuItem {
        text: qsTr("Mark all as read")
        enabled: root.showMarkAsRead
        visible: enabled
        onClicked: markedAsRead()
    }

    MenuItem {
        text: qsTr("Mark all as unread")
        enabled: root.showMarkAsUnread
        visible: enabled
        onClicked: markedAsUnread()
    }

    onActiveChanged: {
        if (active) {
            if (!dm.busy && !fetcher.busy) {
                bar.open = true;
            }
            var lastSync = settings.lastUpdateDate;
            if (lastSync>0)
                label.text = qsTr("Last sync: %1").arg(utils.getHumanFriendlyTimeString(lastSync));
            else
                label.text = qsTr("Not yet synced");
        } else {
            bar.open = false;
        }
    }

    MenuLabel {
        id: label
    }
}
