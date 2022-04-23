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


Dialog {
    id: root

    property bool showBar: false

    allowedOrientations: {
        switch (settings.allowedOrientations) {
        case 1:
            return Orientation.PortraitMask;
        case 2:
            return Orientation.LandscapeMask;
        }
        return Orientation.All;
    }

    ActiveDetector {}

    Column {
        anchors { top: parent.top; left: parent.left; right: parent.right }
        height: app.flickHeight
        Behavior on height {NumberAnimation { duration: 200; easing.type: Easing.OutQuad }}
        clip: true
        spacing: Theme.paddingSmall

        DialogHeader {
            acceptText : qsTr("Yes")
        }

        // Sign in types:
        //  0 - Netvibes
        //  1 - Netvibes with Twitter
        //  2 - Netvibes with FB
        // 10 - Oldreader
        // 20 - Feedly (not supported)
        // 22 - Feedly with FB (not supported)
        // 30 - Tiny Tiny Rss

        Label {
            anchors.left: parent.left; anchors.right: parent.right
            anchors.margins: Theme.paddingLarge;
            wrapMode: Text.WordWrap
            font.pixelSize: Theme.fontSizeLarge
            color: Theme.primaryColor
            text: settings.signinType < 10 ?
                      qsTr("Disconnect Netvibes account?") :
                  settings.signinType < 20 ?
                      qsTr("Disconnect Old Reader account?") :
                  settings.signinType < 30 ?
                      qsTr("Disconnect Feedly account?") :
                  qsTr("Disconnect Tiny Tiny RSS account?")
        }
    }

    onAccepted: {
        settings.signedIn = false;
    }
}
