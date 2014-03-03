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
    property bool signedIn

    onOffLineModeChanged: {
        settings.setOfflineMode(offLineMode);
    }

    Component.onCompleted: {
        offLineMode = settings.getOfflineMode();
        signedIn = settings.getSignedIn();
        db.init();
    }

    Connections {
        target: settings

        onSettingsChanged: {
            offLineMode = settings.getOfflineMode();
            signedIn = settings.getSignedIn();
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

            utils.updateModels();
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
            busyDM.text = "" + remaining + " more items left..."
            if (remaining === 0) {
                busyDM.text = "All done!";
            }
        }

        onCanceled: {
            console.log("DM canceled!");
            busyDM.hide();
        }

        onBusy: {
            console.log("DM busy!");
            busyDM.show("Caching...",true);
        }

        onReady: {
            console.log("DM ready!");
            busyDM.hide();
        }

        onNetworkNotAccessible: {
            notification.show(qsTr("Network connection is unavailable!"));
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

        onCanceled: {
            console.log("Fetcher canceled!");
            busy.hide();
        }

        onReady: {
            console.log("Fetcher ready!");
            //notification.show(qsTr("Sync done!"));
            utils.setTabModel(settings.getNetvibesDefaultDashboard());
            pageStack.replaceAbove(null,Qt.resolvedUrl("TabPage.qml"));
            busy.hide();

            if (!dm.isBusy() && settings.getAutoDownloadOnUpdate())
                dm.startFeedDownload();
        }

        onNetworkNotAccessible: {
            notification.show(qsTr("Network connection is unavailable!"));
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

    BusyBar {
        id: busyDM
        cancelable: true
        onCloseClicked: {
            if (dm.isBusy()) {
                dm.cancel();
            }
        }
    }

    BusyBar {
        id: busy
    }

}
