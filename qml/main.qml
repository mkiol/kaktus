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


ApplicationWindow {
    id: app

    property bool offLineMode

    onOffLineModeChanged: {
        settings.setOfflineMode(offLineMode);
        /*if (offLineMode)
            notification.show(qsTr("Offline mode!"));
        else
            notification.show(qsTr("Online mode!"));*/
    }

    Component.onCompleted: {
        offLineMode = settings.getOfflineMode();
        db.init();
    }

    Connections {
        target: settings

        onSettingsChanged: {
            offLineMode = settings.getOfflineMode();
        }

        onError: {
            console.log("Settings error!");
            console.log("code=" + code);
            notification.show(qsTr("An unknown error occurred! :-("));
            Qt.quit();
        }
    }

    Connections {
        target: db

        onError: {
            console.log("DB error!");
            Qt.quit();
        }

        onEmpty: {
            console.log("DB is empty!");

            utils.setTabModel(settings.getNetvibesDefaultDashboard());
            pageStack.replaceAbove(null,Qt.resolvedUrl("TabPage.qml"));
        }

        onNotEmpty: {
            console.log("DB is not empty!");

            utils.setTabModel(settings.getNetvibesDefaultDashboard());
            pageStack.replaceAbove(null,Qt.resolvedUrl("TabPage.qml"));
        }
    }

    Connections {
        target: dm

        onProgress: {
            console.log("DM progress: " + remaining);
            busy.text = "" + remaining + " more items left..."
            if (remaining === 0) {
                busy.text = "All done!";
            }
        }

        onBusy: {
            console.log("DM busy!");
            busy.show("Caching...", true);
        }

        onReady: {
            console.log("DM ready!");
            busy.hide();
        }

        /*onError: {
            console.log("DM error!");
            console.log("code=" + code);

            if (dm.isBusy()) {
                dm.cancel();
            }
            busy.hide();

            if (code < 400) {
                return;
            }

            if (code >= 500) {
                pageStack.push(Qt.resolvedUrl("ErrorPage.qml"),{"message": "DM error, code: " + code});
            }
        }*/
    }

    Connections {
        target: fetcher

        onReady: {
            console.log("Fetcher ready!");
            notification.show(qsTr("Sync done!"));
            utils.setTabModel(settings.getNetvibesDefaultDashboard());
            pageStack.replaceAbove(null,Qt.resolvedUrl("TabPage.qml"));

            if (!dm.isBusy() && settings.getAutoDownloadOnUpdate() ) {
                dm.startFeedDownload();
            } else {
                busy.hide();
            }
        }

        onError: {
            console.log("Fetcher error");
            console.log("code=" + code);
            busy.hide();

            if (code < 400) {
                return;
            }

            if (code >= 400 && code < 500) {
                if (code == 402)
                    notification.show(qsTr("User & Password do not match!"));
                // Sign in
                pageStack.push(Qt.resolvedUrl("SignInDialog.qml"),{"code": code});
            } else {
                // Unknown error
                notification.show(qsTr("An unknown error occurred! :-("));
            }
        }

        onErrorCheckingCredentials: {
            console.log("Fetcher checking error");
            console.log("code=" + code);
            notification.show(qsTr("User & Password do not match!"));
            busy.hide();
        }

        onCredentialsValid: {
            console.log("Fetcher credentials valid");
            notification.show(qsTr("Successfully Signed In!"));
            busy.hide();
        }

        onProgress: {
            console.log("Fetcher progress: " + current + "/" + total);
            //busy.text = "Fetching data... " + Math.floor((current / total) * 100) + "%";
            busy.text = "Receiving data... ";
            busy.progress = current / total;
        }

        onInitiating: {
            console.log("Fetcher init started!");
            busy.text = "Initiating...";
        }

        onUpdating: {
            console.log("Fetcher update started!");
            busy.text = "Updating...";
        }

        onUploading: {
            console.log("Fetcher uploading!");
            busy.text = "Sending data to Netvibes...";
        }

        onCheckingCredentials: {
            console.log("Fetcher checking credentials!");
            busy.text = "Signing in...";
        }

        onBusy: {
            console.log("Fetcher busy!");
            busy.show("", false);
        }

    }

    Notification {
        id: notification
    }

    Image {
        anchors {
            right: parent.right
            bottom: parent.bottom
            margins: Theme.paddingSmall
        }
        source: offLineMode ? "image://theme/icon-status-wlan-no-signal" : "image://theme/icon-status-wlan-4"
        height: 48
        width: 48
        smooth: true

        MouseArea {
            anchors.fill: parent
            onClicked: {
                offLineMode = !offLineMode;
            }
        }
    }

    BusyBar {
        id: busy
        onCloseClicked: {
            console.log("cancel!");
            if (dm.isBusy()) {
                dm.cancel();
            }
        }
    }

}
