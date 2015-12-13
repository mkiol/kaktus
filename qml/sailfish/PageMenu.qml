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
    property bool showShowOnlyUnread: false

    signal markedAsRead
    signal markedAsUnread

    /*MenuItem {
        text: settings.offlineMode ? qsTr("Set to: Online") : qsTr("Set to: Offline")

        onClicked: {
            settings.offlineMode = !settings.offlineMode;
        }
    }*/

    /*MenuItem {
        text: qsTr("Test")
        onClicked: {
            //pageStack.push(Qt.resolvedUrl("DebugPage.qml"));
            pageStack.push(Qt.resolvedUrl("DebugPage.qml"));
        }
    }*/

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
        text: enabled ? qsTr("Sync") : qsTr("Busy...")
        visible: root.showSync

        onClicked: fetcher.update()
        enabled: !fetcher.busy && !dm.busy && !dm.removerBusy
    }

    MenuItem {
        text: settings.showOnlyUnread ? qsTr("Showing: only unread") : qsTr("Showing: all articles")
        enabled: root.showShowOnlyUnread
        visible: enabled
        onClicked: {
            settings.showOnlyUnread = !settings.showOnlyUnread;
        }
    }

    MenuItem {
        text: qsTr("Mark all as unread")
        enabled: root.showMarkAsUnread
        visible: enabled
        onClicked: markedAsUnread()
    }

    MenuItem {
        text: qsTr("Mark all as read")
        enabled: root.showMarkAsRead
        visible: enabled
        onClicked: markedAsRead()
    }

    onActiveChanged: {
        if (active) {

            bar.hideAndDisable();

            /*if (!settings.signedIn) {
               label.text = qsTr("You are not signed in");
               return;
            }*/

            /*console.log("fetcher.busy",fetcher.busy);
            console.log("fetcher.busyType",fetcher.busyType);
            console.log("dm.busy",dm.busy);
            console.log("dm.removerBusy",dm.removerBusy);*/
                
            var lastSync = settings.lastUpdateDate;
            if (lastSync>0)
                label.text = qsTr("Last sync: %1").arg(utils.getHumanFriendlyTimeString(lastSync));
            else
                label.text = qsTr("You have never synced");

        } else {

            bar.openable = true;
        }
    }

    MenuLabel {
        id: label
    }
}
