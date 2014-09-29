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

    property bool showBar: false

    allowedOrientations: {
        switch (settings.allowedOrientations) {
        case 1:
            return Orientation.Portrait;
        case 2:
            return Orientation.Landscape;
        }
        return Orientation.Landscape | Orientation.Portrait;
    }

    ActiveDetector {
        onActivated: tabModel.updateFlags()
        onInit: bar.flick = listView
    }

    SilicaListView {
        id: listView

        anchors { top: parent.top; left: parent.left; right: parent.right }

        height: app.flickHeight

        clip:true

        PageMenu {
            id: menu
            showAbout: true
            showMarkAsRead: false
            showMarkAsUnread: false
        }

        header: PageHeader {
            title: qsTr("")
        }

        ViewPlaceholder {
            enabled: listView.count < 1
            text: qsTr("You are not signed in")

            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.secondaryHighlightColor
                text: fetcher.busy ? qsTr("Wait until Sync finish") : settings.signedIn ? "" : qsTr("You are not signed in")
            }
        }

        VerticalScrollDecorator {
            flickable: listView
        }
    }
}
