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

    function getHumanFriendlyTimeString(date) {
        var delta = Math.floor(Date.now()/1000-date);
        if (delta===0) {
            return qsTr("just now");
        }
        if (delta===1) {
            return qsTr("1 second ago");
        }
        if (delta<5) {
            return qsTr("%1 seconds ago","less than 5 seconds").arg(delta);
        }
        if (delta<60) {
            return qsTr("%1 seconds ago","more or equal 5 seconds").arg(delta);
        }
        if (delta<120) {
            return qsTr("1 minute ago");
        }
        if (delta<300) {
            return qsTr("%1 minutes ago","less than 5 minutes").arg(Math.floor(delta/60));
        }
        if (delta<3600) {
            return qsTr("%1 minutes ago","more or equal 5 minutes").arg(Math.floor(delta/60));
        }
        if (delta<7200) {
            return qsTr("1 hour ago");
        }
        if (delta<18000) {
            return qsTr("%1 hours ago","less than 5 hours").arg(Math.floor(delta/3600));
        }
        if (delta<86400) {
            return qsTr("%1 hours ago","more or equal 5 hours").arg(Math.floor(delta/3600));
        }
        if (delta<172800) {
            return qsTr("yesterday");
        }
        if (delta<432000) {
            return qsTr("%1 days ago","less than 5 days").arg(Math.floor(delta/86400));
        }
        if (delta<604800) {
            return qsTr("%1 days ago","more or equal 5 days").arg(Math.floor(delta/86400));
        }
        if (delta<1209600) {
            return qsTr("1 week ago");
        }
        if (delta<2419200) {
            return qsTr("%1 weeks ago").arg(Math.floor(delta/604800));
        }
        return Qt.formatDateTime(new Date(date*1000),"dddd, d MMMM yy");
    }

    cover: CoverPage {}

    Component.onCompleted: {
        db.init();
    }

    Connections {
        target: settings

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
            dm.removeCache();
            utils.updateModels();
            utils.setTabModel(settings.getNetvibesDefaultDashboard());
            pageStack.replaceAbove(null,Qt.resolvedUrl("TabPage.qml"));
        }

        onNotEmpty: {
            utils.setTabModel(settings.getNetvibesDefaultDashboard());
            pageStack.replaceAbove(null,Qt.resolvedUrl("TabPage.qml"));
        }
    }

    Connections {
        target: dm

        onProgress: {
            //console.log("DM progress: " + remaining);
            busyDM.text = qsTr("%1 more items left...").arg(remaining);
            if (remaining === 0) {
                busyDM.text = qsTr("All done!");
            }
        }

        onCanceled: {
            busyDM.hide();
        }

        onBusy: {
            busyDM.show(qsTr("Caching..."),true);
        }

        onReady: {
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
            busy.hide();
        }

        onReady: {
            //notification.show(qsTr("Sync done!"));
            utils.setTabModel(settings.getNetvibesDefaultDashboard());
            pageStack.replaceAbove(null,Qt.resolvedUrl("TabPage.qml"));
            busy.hide();

            //console.log("DM busy: "+dm.isBusy());
            if (settings.getAutoDownloadOnUpdate())
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
            notification.show(qsTr("User & Password do not match!"));
            busy.hide();
        }

        onCredentialsValid: {
            notification.show(qsTr("Successfully Signed In!"));
            busy.hide();
        }

        onProgress: {
            busy.text = qsTr("Receiving data... ");
            busy.progress = current / total;
        }

        onInitiating: {
            busy.text = qsTr("Initiating...");
        }

        onUpdating: {
            busy.text = qsTr("Updating...");
        }

        onUploading: {
            busy.text = qsTr("Sending data to Netvibes...");
        }

        onCheckingCredentials: {
            busy.text = qsTr("Signing in...");
        }

        onBusy: {
            busy.show("", true);
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
        cancelable: true
        onCloseClicked: {
            if (fetcher.isBusy()) {
                fetcher.cancel();
            }
        }
    }

}
