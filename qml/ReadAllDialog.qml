/* Copyright (C) 2016-2022 Michal Kosciesza <michal@mkiol.net>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

import QtQuick 2.0
import Sailfish.Silica 1.0

import harbour.kaktus.Settings 1.0

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
                    return settings.viewMode === Settings.AllEntries ? qsTr("Mark all your articles as read?") :
                                settings.viewMode === Settings.SavedEntries ?
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
