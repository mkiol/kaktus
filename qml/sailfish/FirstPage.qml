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

    Component.onCompleted: {
        timer.start();
    }

    Timer {
        id: timer
        interval: 3000
        onTriggered: {
            if (settings.signedIn || fetcher.busy || dm.busy) {
                timer.stop();
                menu.busy = false;
                help.open = false;
            } else {
                menu.busy = true;
                help.open = true;
            }
        }
    }

    Label {
        id: help
        property bool open: false
        anchors.left: parent.left; anchors.right: parent.right
        anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
        anchors.top: parent.top; anchors.topMargin: 2*Theme.paddingLarge
        font.pixelSize: Theme.fontSizeLarge
        color: Theme.highlightColor
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        opacity: open ? 1.0 : 0.0
        visible: opacity > 0.0
        Behavior on opacity { FadeAnimation {duration: 400} }

        text: qsTr("To sign in and do feeds synchronisation with Netvibes, pull down and select Sync.")
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

            onActiveChanged: {
                if (active) {
                    timer.stop();
                    menu.busy = false;
                    help.open = false;
                } else {
                    //if (!settings.signedIn)
                    //    timer.start();
                }
            }
        }

        ViewPlaceholder {
            id: placeholder
            enabled: listView.count < 1
            text: settings.signedIn ? qsTr("Signed in") : qsTr("Not signed in")
        }
        Label {
            visible: placeholder.enabled
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: placeholder.bottom; anchors.bottomMargin: Theme.paddingMedium
            font.pixelSize: Theme.fontSizeSmall
            color: Theme.secondaryHighlightColor
            text: fetcher.busy ? qsTr("Wait until Sync finish.") : ""
        }

        VerticalScrollDecorator {
            flickable: listView
        }
    }
}
