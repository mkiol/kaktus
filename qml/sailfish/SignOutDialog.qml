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
            return Orientation.Portrait;
        case 2:
            return Orientation.Landscape;
        }
        return Orientation.Landscape | Orientation.Portrait;
    }

    ActiveDetector {}

    Column {

        anchors { top: parent.top; left: parent.left; right: parent.right }

        clip:true

        height: app.flickHeight

        spacing: Theme.paddingSmall

        DialogHeader {
            //title: qsTr("Sign out")
            acceptText : qsTr("Yes")
        }

        Label {
            anchors.left: parent.left; anchors.right: parent.right
            anchors.margins: Theme.paddingLarge;
            wrapMode: Text.WordWrap
            font.pixelSize: Theme.fontSizeLarge
            color: Theme.primaryColor
            text: qsTr("Disconnect Kaktus from your Netvibes account?")
        }
    }

    onAccepted: {
        settings.signedIn = false;
    }

}
