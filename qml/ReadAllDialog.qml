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
    property int type: 2 // tabs=0, feeds=1, entries=2

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
            //title: qsTr("Mark as read")
            acceptText : qsTr("Yes")
        }

        Label {
            anchors.left: parent.left; anchors.right: parent.right
            anchors.margins: Theme.paddingLarge;
            wrapMode: Text.WordWrap
            font.pixelSize: Theme.fontSizeLarge
            color: Theme.primaryColor
            text: {
                if (type==0) {
                    return qsTr("Mark all tabs as read?");
                }
                if (type==1) {
                    return qsTr("Mark feeds as read?");
                }
                if (type==2) {
                    return settings.viewMode == 3 ? qsTr("Mark all your articles as read?") :
                                settings.viewMode == 4 ?
                                    settings.signinType<10 || settings.signinType>=20 ? qsTr("Mark all saved articles as read?") : qsTr("Mark all starred articles as read?") :
                                    qsTr("Mark articles as read?");
                }
            }
        }
    }

    onAccepted: {
        if (type==0) {
            tabModel.setAllAsRead();
            return;
        }
        if (type==1) {
            feedModel.setAllAsRead();
            return;
        }
        if (type==2) {
            entryModel.setAllAsRead();
            return;
        }
    }
}
