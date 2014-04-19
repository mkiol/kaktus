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
            root.unread = utils.getUnreadItemsCount();
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

    Label {
        anchors.centerIn: parent
        font.pixelSize: Theme.fontSizeMedium
        font.family: Theme.fontFamilyHeading
        color: Theme.highlightColor
        text: qsTr("Not signed in")
        visible: !settings.signedIn
    }

    Column {
        anchors.top: parent.top; anchors.left: parent.left; anchors.right: parent.right;
        anchors.margins: Theme.paddingMedium

        spacing: Theme.paddingMedium

        visible: !dm.busy && !fetcher.busy && settings.signedIn && settings.signedIn

        /*Item {
            anchors.left: parent.left; anchors.right: parent.right;
            height: Math.max(unreadDesc.height,unreadlabel.height)

            Label {
                id: unreadlabel
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                text: root.unread
                color: Theme.highlightColor
                font.pixelSize: root.unread>999 ? Theme.fontSizeLarge : Theme.fontSizeHuge
                font.family: Theme.fontFamilyHeading

            }

            Label {
                id: unreadDesc
                font.pixelSize: Theme.fontSizeExtraSmall
                font.family: Theme.fontFamilyHeading
                anchors.verticalCenter: parent.verticalCenter
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
            }

            Column {
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: unreadlabel.right; anchors.right: parent.right;
                spacing: 0

                Label {
                    font.pixelSize: Theme.fontSizeExtraSmall
                    font.family: Theme.fontFamilyHeading
                    text: qsTr("Uread")
                }

                Label {
                    font.pixelSize: Theme.fontSizeExtraSmall
                    font.family: Theme.fontFamilyHeading
                    text: qsTr("items")
                }
            }
        }*/

        Column {
            anchors.left: parent.left; anchors.right: parent.right;
            visible: root.unread>0

            Label {
                id: unreadlabel
                text: root.unread>0 ? root.unread : qsTr("All read")
                color: Theme.highlightColor
                font.pixelSize: root.unread>0 ? Theme.fontSizeHuge : Theme.fontSizeLarge
                font.family: Theme.fontFamilyHeading
                wrapMode: Text.Wrap
                anchors.left: parent.left; anchors.right: parent.right;
                horizontalAlignment: Text.AlignHCenter
            }

            Label {
                visible: root.unread>0
                font.pixelSize: Theme.fontSizeSmall
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
                wrapMode: Text.Wrap
                anchors.left: parent.left; anchors.right: parent.right;
                horizontalAlignment: Text.AlignHCenter
            }
        }

        Rectangle {
            anchors.left: parent.left; anchors.right: parent.right;
            height: 2
            color: Theme.primaryColor
            opacity: 0.1
            visible: root.unread>0
        }

        Column {
            anchors.left: parent.left; anchors.right: parent.right;
            visible: settings.lastUpdateDate>0

            Label {
                font.pixelSize: Theme.fontSizeSmall
                font.family: Theme.fontFamilyHeading
                text: qsTr("Last sync")
                wrapMode: Text.Wrap
                anchors.left: parent.left; anchors.right: parent.right;
                horizontalAlignment: Text.AlignHCenter

            }

            Label {
                id: lastupdateLabel
                font.pixelSize: Theme.fontSizeSmall
                font.family: Theme.fontFamilyHeading
                color: Theme.highlightColor
                wrapMode: Text.Wrap
                anchors.left: parent.left; anchors.right: parent.right;
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }

    Column {
        anchors.top: parent.top; anchors.left: parent.left; anchors.right: parent.right;
        anchors.margins: Theme.paddingMedium
        visible: dm.busy || fetcher.busy
        spacing: Theme.paddingMedium

        Label {
            id: label
            font.pixelSize: Theme.fontSizeMedium
            font.family: Theme.fontFamilyHeading
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Label {
            id: progressLabel
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: Theme.fontSizeHuge
            font.family: Theme.fontFamilyHeading
            color: Theme.highlightColor
        }
    }

    CoverActionList {
        enabled: !dm.busy && !fetcher.busy
        CoverAction {
            iconSource: "image://theme/icon-cover-sync"
            onTriggered: fetcher.update();
        }
    }

    CoverActionList {
        enabled: dm.busy || fetcher.busy
        CoverAction {
            iconSource: "image://theme/icon-cover-cancel"
            onTriggered: {
                dm.cancel();
                fetcher.cancel();
            }
        }
    }

    Connections {
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
    }

    Connections {
        target: dm

        onProgress: {
            if (!fetcher.busy) {
                label.text = qsTr("Caching");
                progressLabel.text = remaining;
            }
        }
    }
}


