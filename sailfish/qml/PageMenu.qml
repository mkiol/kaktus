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
    //property bool showSync: true
    //property bool showShowOnlyUnread: false
    property bool showNetwork: true

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
        text: settings.offlineMode ? qsTr("Network mode: offline") : qsTr("Network mode: online")
        visible: root.showNetwork
        enabled: !settings.offlineMode || (settings.offlineMode && dm.online)

        onClicked: {
            if (settings.offlineMode) {
                if (dm.online)
                    settings.offlineMode = false;
                else
                    notification.show(qsTr("Can't switch to online mode because network is disconnected."));
            } else {
                settings.offlineMode = true;
            }
        }
    }

    /*MenuItem {
        text: settings.showOnlyUnread ? qsTr("Showing: only unread") : qsTr("Showing: all articles")
        enabled: root.showShowOnlyUnread
        visible: enabled
        onClicked: {
            settings.showOnlyUnread = !settings.showOnlyUnread;
        }
    }*/

    /*MenuItem {
        text: enabled ? qsTr("Sync") : qsTr("Busy...")
        visible: root.showSync

        onClicked: fetcher.update()
        enabled: !fetcher.busy && !dm.busy && !dm.removerBusy
    }*/

    onActiveChanged: {
        if (active) {
            bar.hideAndDisable();
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
