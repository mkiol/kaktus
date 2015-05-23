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


CoverBackground {
    id: root

    property int unread: 0
    property bool active: status == Cover.Active

    onStatusChanged: {
        if (status==Cover.Active) {
            root.unread = utils.countUnread();
            lastupdateLabel.text = utils.getHumanFriendlyTimeString(settings.lastUpdateDate);
            timer.setInterval();
            timer.restart()
        }
    }

    Timer {
        id: timer

        running: active

        function setInterval() {
            var delta = Math.floor(Date.now()/1000-settings.lastUpdateDate);
            if (delta<60) {
                timer.interval = 1000;
                return;
            }
            if (delta<3600) {
                timer.interval = 60000;
                return;
            }
            timer.interval = 3600000;
        }

        onTriggered: {
            if (active) {
                lastupdateLabel.text = utils.getHumanFriendlyTimeString(settings.lastUpdateDate);
                setInterval();
                restart();
            }
        }
    }

    Image {
        id: image
        source: "cover.png"
        opacity: 0.2
        anchors.bottom: parent.bottom
    }

    // Not signed In
    Label {
        visible: !settings.signedIn

        anchors.top: parent.top; anchors.left: parent.left; anchors.right: parent.right;
        anchors.margins: Theme.paddingLarge
        font.pixelSize: Theme.fontSizeMedium
        font.family: Theme.fontFamilyHeading

        wrapMode: Text.Wrap
        horizontalAlignment: Text.AlignHCenter

        text: qsTr("Not signed in")
    }

    // Singned In and idle
    Column {
        visible: !dm.busy && !app.fetcherBusyStatus && settings.signedIn && settings.signedIn

        anchors.top: parent.top; anchors.topMargin: Theme.paddingMedium
        anchors.left: parent.left; anchors.right: parent.right;
        anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge

        spacing: Theme.paddingMedium

        Item {
            anchors.left: parent.left; anchors.right: parent.right;
            height: Math.max(unreadDesc.height,unreadlabel.height)
            visible: root.unread>0

            Label {
                id: unreadlabel
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                text: root.unread
                color: Theme.primaryColor
                font.pixelSize: root.unread>999 ? Theme.fontSizeLarge : Theme.fontSizeHuge
                font.family: Theme.fontFamilyHeading
            }

            Label {
                id: unreadDesc

                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: Theme.fontSizeExtraSmall
                font.family: Theme.fontFamilyHeading
                anchors.left: unreadlabel.right; anchors.right: parent.right;
                anchors.leftMargin: Theme.paddingMedium
                text: {
                    if (root.unread==0)
                        return qsTr("All read");
                    if (root.unread==1)
                        return qsTr("Unread item");
                    if (root.unread<5)
                        return qsTr("Unread items","less than 5 articles are unread");
                    return qsTr("Unread items","more or equal 5 articles are unread");
                }
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignLeft
                truncationMode: TruncationMode.Fade
                maximumLineCount: 2
            }
        }

        Label {
            visible: root.unread==0

            font.pixelSize: Theme.fontSizeMedium
            font.family: Theme.fontFamilyHeading
            anchors.left: parent.left; anchors.right: parent.right;
            text: qsTr("All read");
            wrapMode: Text.Wrap
            horizontalAlignment: Text.AlignLeft
            truncationMode: TruncationMode.Fade
            maximumLineCount: 2
        }

        /*Column {
            anchors.left: parent.left; anchors.right: parent.right;
            visible: root.unread>0

            Label {
                font.pixelSize: Theme.fontSizeMedium
                font.family: Theme.fontFamilyHeading
                text: {
                    if (root.unread==0)
                        return qsTr("All read");
                    if (root.unread==1)
                        return qsTr("Unread item");
                    if (root.unread<5)
                        return qsTr("Unread items","less than 5 articles are unread");
                    return qsTr("Unread items","more or equal 5 articles are unread");
                }
                wrapMode: Text.NoWrap
                anchors.left: parent.left; anchors.right: parent.right;
                horizontalAlignment: Text.AlignHCenter
                elide: Text.ElideRight
            }

            Label {
                id: unreadlabel
                text: root.unread
                color: Theme.highlightColor
                font.pixelSize: Theme.fontSizeHuge
                font.family: Theme.fontFamilyHeading
                wrapMode: Text.Wrap
                anchors.left: parent.left; anchors.right: parent.right;
                horizontalAlignment: Text.AlignHCenter
            }
        }*/

        Column {
            anchors.left: parent.left; anchors.right: parent.right;
            visible: settings.lastUpdateDate>0

            /*Label {
                font.pixelSize: Theme.fontSizeMedium
                font.family: Theme.fontFamilyHeading
                text: qsTr("Last sync")
                wrapMode: Text.Wrap
                anchors.left: parent.left; anchors.right: parent.right;
                horizontalAlignment: Text.AlignHCenter
            }*/

            Label {
                id: lastupdateLabel
                font.pixelSize: Theme.fontSizeLarge
                font.family: Theme.fontFamilyHeading
                color: Theme.highlightColor
                wrapMode: Text.Wrap
                anchors.left: parent.left; anchors.right: parent.right;
                horizontalAlignment: Text.AlignLeft
            }
        }
    }

    // Busy
    Column {
        anchors.top: parent.top; anchors.topMargin: Theme.paddingLarge
        anchors.left: parent.left; anchors.right: parent.right;
        anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge

        visible: dm.busy || app.fetcherBusyStatus
        spacing: Theme.paddingMedium

        Label {
            id: label
            font.pixelSize: Theme.fontSizeMedium
            font.family: Theme.fontFamilyHeading
            wrapMode: Text.Wrap
            anchors.left: parent.left; anchors.right: parent.right;
            horizontalAlignment: Text.AlignLeft
        }

        Label {
            id: progressLabel
            font.pixelSize: Theme.fontSizeHuge
            font.family: Theme.fontFamilyHeading
            color: Theme.highlightColor
            wrapMode: Text.Wrap
            anchors.left: parent.left; anchors.right: parent.right;
            horizontalAlignment: Text.AlignLeft
        }
    }

    CoverActionList {
        enabled: !dm.busy && !app.fetcherBusyStatus
        CoverAction {
            iconSource: "image://theme/icon-cover-sync"
            onTriggered: fetcher.update();
        }
    }

    CoverActionList {
        enabled: dm.busy || app.fetcherBusyStatus
        CoverAction {
            iconSource: "image://theme/icon-cover-cancel"
            onTriggered: {
                dm.cancel();
                fetcher.cancel();
            }
        }
    }

    function connectFetcher() {
        if (typeof fetcher === 'undefined')
            return;
        fetcher.progress.connect(fetcherProgress);
        fetcher.busyChanged.connect(fetcherBusyChanged);
    }

    function disconnectFetcher() {
        if (typeof fetcher === 'undefined')
            return;
        fetcher.progress.disconnect(fetcherProgress);
        fetcher.busyChanged.disconnect(fetcherBusyChanged);
    }

    function fetcherProgress(current, total) {
        label.text = qsTr("Syncing");
        progressLabel.text = Math.floor((current/total)*100)+"%";
    }

    function fetcherBusyChanged() {
        switch(fetcher.busyType) {
        case 1:
            label.text = qsTr("Initiating");
            progressLabel.text = "0%"
            break;
        case 2:
            label.text = qsTr("Updating")
            progressLabel.text = "0%"
            break;
        case 3:
            label.text = qsTr("Signing in")
            progressLabel.text = "";
            break;
        }

        if (!fetcher.busy && active) {
            root.unread = utils.getUnreadItemsCount();
            lastupdateLabel.text = utils.getHumanFriendlyTimeString(settings.lastUpdateDate);
            timer.setInterval();
            timer.restart();
        }
    }

    /*Connections {
        target: fetcher

        onProgress: {
            label.text = qsTr("Syncing");
            progressLabel.text = Math.floor((current/total)*100)+"%";
        }

        onBusyChanged: {
            switch(fetcher.busyType) {
            case 1:
                label.text = qsTr("Initiating");
                progressLabel.text = "0%"
                break;
            case 2:
                label.text = qsTr("Updating")
                progressLabel.text = "0%"
                break;
            case 3:
                label.text = qsTr("Signing in")
                progressLabel.text = "";
                break;
            }

            if (!fetcher.busy && active) {
                root.unread = utils.getUnreadItemsCount();
                lastupdateLabel.text = utils.getHumanFriendlyTimeString(settings.lastUpdateDate);
                timer.setInterval();
                timer.restart();
            }
        }
    }*/

    Connections {
        target: dm

        onProgress: {
            if (!fetcher.fetcherBusyStatus) {
                label.text = qsTr("Caching");
                progressLabel.text = remaining;
            }
        }
    }
}


