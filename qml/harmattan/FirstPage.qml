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

import QtQuick 1.1
import com.nokia.meego 1.0

import "Theme.js" as Theme

Page {
    id: root

    tools: MainToolbar {}

    ActiveDetector {}    

    orientationLock: {
        switch (settings.allowedOrientations) {
        case 1:
            return PageOrientation.LockPortrait;
        case 2:
            return PageOrientation.LockLandscape;
        }
        return PageOrientation.Automatic;
    }

    Timer {
        id: timer
        interval: 3000
        onTriggered: {
            if (settings.signedIn || fetcher.busy || dm.busy) {
                timer.stop();
                help.open = false;
                //selector.open = false;
            } else {
                help.open = true;
                //selector.open = true;
            }
        }
    }

    Connections {
        target: settings
        onSignedInChanged: {
            help.open = false;
        }
    }

    onStatusChanged: {
        if (status===PageStatus.Active) {
            timer.start();
        }
        if (status===PageStatus.Deactivating||status===PageStatus.NotActive) {
            help.open=false;
            //selector.open = false;
        }
    }

    Item {
        id: help
        property bool open: false
        anchors.fill: parent
        opacity: open ? 1.0 : 0.0
        visible: opacity > 0.0
        Behavior on opacity { NumberAnimation {duration: 400} }

        Label {
            anchors.left: parent.left; anchors.right: parent.right
            anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
            anchors.top: parent.top; anchors.topMargin: 2*Theme.paddingLarge
            font.pixelSize: Theme.fontSizeLarge
            color: Theme.highlightColor
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter

            text: qsTr("Press the button below to sign in and do feeds sync with Netvibes.")
        }
    }

    ListView {
        id: listView

        anchors.fill: parent

        clip: true

        PullBar {}
    }

    ViewPlaceholder {
        enabled: listView.count < 1
        text: settings.signedIn ? qsTr("Signed in") : qsTr("Not signed in")
        secondaryText: fetcher.busy ? qsTr("Wait until Sync finish.") : ""
    }

    ScrollDecorator { flickableItem: listView }
}
