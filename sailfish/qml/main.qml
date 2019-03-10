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
import QtQuick.Window 2.0

ApplicationWindow {
    id: app

    property bool progress: false
    property int oldViewMode

    readonly property bool isTablet: Screen.sizeCategory === Screen.Large || Screen.sizeCategory === Screen.ExtraLarge
    readonly property bool isNetvibes: settings.signinType >= 0 && settings.signinType < 10
    readonly property bool isOldReader: settings.signinType >= 10 && settings.signinType < 20
    readonly property bool isTTRss: settings.signinType >= 30 && settings.signinType < 40
    readonly property int stdHeight: orientation==Orientation.Portrait ? Theme.itemSizeMedium : 0.8 * Theme.itemSizeMedium

    cover: CoverPage {}

    Component.onCompleted: {
        db.init();

        if (settings.autoOffline) {
            if (dm.online) {
                if (settings.offlineMode) {
                    settings.offlineMode = false
                }
            } else {
                if (!settings.offlineMode) {
                    settings.offlineMode = true
                }
            }
        }
    }

    function hideBar() {
        bar.hide();
    }

    function showBar() {
        bar.show();
    }

    function resetView() {
        if (!settings.signedIn) {
            pageStack.replaceAbove(null,Qt.resolvedUrl("FirstPage.qml"));
            return;
        }

        // Reconnect fetcher
        if (typeof fetcher === 'undefined') {
            var type = settings.signinType;

            if (type < 10)
                reconnectFetcher(1);
            else if (type < 20)
                reconnectFetcher(2);
            else if (type < 30)
                reconnectFetcher(3);
            else if (type < 40)
                reconnectFetcher(4);
        }

        utils.setRootModel();

        var newViewMode = settings.viewMode;
        if ((oldViewMode == 3 || oldViewMode == 4 || oldViewMode == 5 || oldViewMode == 6 || oldViewMode == 7) &&
            (newViewMode == 3 || newViewMode == 4 || newViewMode == 5 || newViewMode == 6 || newViewMode == 7)) {
            // No need to change stack
            app.progress = false;
        } else {
            pageStack.busyChanged.connect(resetViewDone);

            switch (settings.viewMode) {
            case 0:
            case 1:
                pageStack.replaceAbove(null,Qt.resolvedUrl("TabPage.qml"));
                break;
            case 2:
                pageStack.replaceAbove(null,Qt.resolvedUrl("FeedPage.qml"),{"title": qsTr("Feeds")});
                break;
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
                pageStack.replaceAbove(null,Qt.resolvedUrl("EntryPage.qml"));
                break;
            }
        }

        oldViewMode = newViewMode;
    }

    function resetViewDone() {
        if (!pageStack.busy) {
            pageStack.busyChanged.disconnect(resetViewDone);
            app.progress = false;
        }
    }

    Connections {
        target: settings

        onError: {
            console.log("Settings error! code=" + code);
            Qt.quit();
        }

        onDashboardInUseChanged: {
            resetView();
            //notification.show(qsTr("Dashboard changed!"));
        }

        onViewModeChanged: {
            resetView();
            //notification.show(qsTr("Browsing mode changed!"));
        }

        onSignedInChanged: {
            if (!settings.signedIn) {
                //notification.show(qsTr("Signed out!"));
                fetcher.cancel(); dm.cancel();
                db.init();
            } else {
                /*if(!settings.helpDone)
                    guide.showDelayed();*/
            }
        }

        onShowBroadcastChanged: {
            // If social features disabled, viemode != 6
            if (!settings.showBroadcast && settings.viewMode == 6) {
                settings.viewMode = 4;
            }
        }
    }

    Connections {
        target: db

        onError: {
            console.log("DB error! code="+code);

            if (code==511) {
                notification.show(qsTr("Restart the app to rebuild cache data."), qsTr("Something went wrong!"));
                return;
            }

            Qt.quit();
        }

        onEmpty: {
            dm.removeCache();
            if (settings.viewMode!=0)
                settings.viewMode=0;
            else
                resetView();
        }

        onNotEmpty: {
            resetView()
        }
    }

    Connections {
        target: dm

        onProgress: {
            if (!app.fetcherBusyStatus) {
                if (current > 0 && total != 0) {
                    bar.progressText= qsTr("Caching... %1 of %2").arg(current).arg(total);
                    bar.progress = current / total;
                } else {
                    bar.progressText = qsTr("Caching...");
                }
            }
        }

        onOnlineChanged: {
            if (settings.autoOffline) {
                if (dm.online) {
                    if (settings.offlineMode) {
                        //notification.show(qsTr("Enabling online mode because network is connected."));
                        settings.offlineMode = false
                    }
                } else {
                    if (!settings.offlineMode) {
                        //notification.show(qsTr("Enabling offline mode because network is disconnected."));
                        settings.offlineMode = true
                    }
                }
            }
        }

        onNetworkNotAccessible: {
            notification.show(qsTr("Download has failed because network is disconnected."));
        }

        onRemoverProgressChanged: {
            bar.progress = current / total;
            bar.progressText = qsTr("Removing cache data... %1 of %2").arg(current).arg(total);
        }
    }

    function reconnectFetcher(type) {
        disconnectFetcher();
        cover.disconnectFetcher();
        utils.resetFetcher(type);
        connectFetcher();
        cover.connectFetcher();
    }

    function connectFetcher() {
        if (typeof fetcher === 'undefined')
            return;
        fetcher.ready.connect(fetcherReady);
        fetcher.newAuthUrl.connect(fetcherNewAuthUrl);
        fetcher.errorGettingAuthUrl.connect(fetcherErrorGettingAuthUrl);
        fetcher.networkNotAccessible.connect(fetcherNetworkNotAccessible);
        fetcher.error.connect(fetcherError);
        fetcher.errorCheckingCredentials.connect(fetcherErrorCheckingCredentials);
        fetcher.credentialsValid.connect(fetcherCredentialsValid);
        fetcher.progress.connect(fetcherProgress);
        fetcher.uploadProgress.connect(fetcherUploadProgress);
        fetcher.uploading.connect(fetcherUploading);
        fetcher.busyChanged.connect(fetcherBusyChanged);
        fetcher.canceled.connect(fetcherCanceled);
    }

    function disconnectFetcher() {
        if (typeof fetcher === 'undefined')
            return;
        fetcher.ready.disconnect(fetcherReady);
        fetcher.newAuthUrl.disconnect(fetcherNewAuthUrl);
        fetcher.errorGettingAuthUrl.disconnect(fetcherErrorGettingAuthUrl);
        fetcher.networkNotAccessible.disconnect(fetcherNetworkNotAccessible);
        fetcher.error.disconnect(fetcherError);
        fetcher.errorCheckingCredentials.disconnect(fetcherErrorCheckingCredentials);
        fetcher.credentialsValid.disconnect(fetcherCredentialsValid);
        fetcher.progress.disconnect(fetcherProgress);
        fetcher.uploadProgress.disconnect(fetcherUploadProgress);
        fetcher.uploading.disconnect(fetcherUploading);
        fetcher.busyChanged.disconnect(fetcherBusyChanged);
        fetcher.canceled.disconnect(fetcherCanceled);
    }

    property bool fetcherBusyStatus: false

    function fetcherReady() {
        //console.log("Fetcher ready");
        resetView();

        switch (settings.cachingMode) {
        case 0:
            return;
        case 1:
            if (dm.isWLANConnected()) {
                dm.startFeedDownload();
            }
            return;
        case 2:
            dm.startFeedDownload();
            return;
        }
    }

    function fetcherNewAuthUrl(url, type) {
        pageStack.push(Qt.resolvedUrl("AuthWebViewPage.qml"),{"url":url,"type":type,"code": 400});
    }

    function fetcherErrorGettingAuthUrl() {
        notification.show(qsTr("Something went wrong. Unable to sign in!"));
    }

    function fetcherNetworkNotAccessible() {
        notification.show(qsTr("Sync failed!\nNetwork connection is unavailable."));
    }

    function fetcherError(code) {
        console.log("Fetcher error, code=" + code);

        if (code < 400)
            return;
        if (code === 700 || (code >= 400 && code < 500)) {
            if (code === 402)
                notification.show(qsTr("The user name or password is incorrect!"));
            else if (code === 404)  // TT-RSS API disabled
                notification.show(qsTr("Access through API is disabled on a server"));
            else if (code === 700)  // SSL error
                notification.show(qsTr("Problem with SSL certificate"));

            // Sign in
            var type = settings.signinType;
            if (type < 10) {
                pageStack.push(Qt.resolvedUrl("NvSignInDialog.qml"),{"code": code});
                return;
            }
            if (type < 20) {
                pageStack.push(Qt.resolvedUrl("OldReaderSignInDialog.qml"),{"code": code});
                return;
            }
            if (type < 40) {
                pageStack.push(Qt.resolvedUrl("TTRssSignInDialog.qml"),{"code": code});
                return;
            }
        } else {
            // Unknown error
            notification.show(qsTr("Unknown error"));
            resetView();
        }
    }

    function fetcherErrorCheckingCredentials() {
        notification.show(qsTr("The user name or password is incorrect."));
    }

    function fetcherCredentialsValid() {
        notification.show(qsTr("You are signed in."));
    }

    function fetcherProgress(current, total) {
        //console.log("fetcherProgress", current, total);
        bar.progressText = qsTr("Receiving data... ");
        bar.progress = current / total;
    }

    function fetcherUploadProgress(current, total) {
        //console.log("fetcherUploadProgress", current, total);
        bar.progressText = qsTr("Sending data...");
        bar.progress = current / total;
    }

    function fetcherUploading() {
        //console.log("fetcherUploading");
        bar.progressText = qsTr("Sending data...");
    }

    function fetcherBusyChanged() {
        //console.log("fetcherBusyChanged:", fetcher.busy, app.fetcherBusyStatus)
        if (app.fetcherBusyStatus != fetcher.busy)
            app.fetcherBusyStatus = fetcher.busy;

        switch(fetcher.busyType) {
        case 1:
            bar.progressText = qsTr("Initiating...");
            bar.progress = 0;
            break;
        case 2:
            bar.progressText = qsTr("Updating...");
            bar.progress = 0;
            break;
        case 3:
            bar.progressText = qsTr("Signing in...");
            bar.progress = 0;
            break;
        case 4:
            bar.progressText = qsTr("Signing in...");
            bar.progress = 0;
            break;
        case 11:
            bar.progressText = qsTr("Waiting for network...");
            bar.progress = 0;
            break;
        case 21:
            bar.progressText = qsTr("Waiting for network...");
            bar.progress = 0;
            break;
        case 31:
            bar.progressText = qsTr("Waiting for network...");
            bar.progress = 0;
            break;
        }
    }

    function fetcherCanceled() {
        resetView();
    }

    Notification {
        id: notification
    }

    property int panelWidth: app.orientation==Orientation.Portrait ? Screen.width : Screen.height
    property int landscapeContentPanelWidth: isTablet ?
                 app.orientation === Orientation.Portrait ? Screen.width-700 : Screen.height-700 :
                 app.orientation === Orientation.Portrait ? Screen.width/2 : Screen.height/2
    property int flickHeight: {
        var size = 0
        if (bar.open)
            size += bar.stdHeight
        return app.orientation === Orientation.Portrait ? Screen.height-size : Screen.width-size;
    }
    property int barX: app.orientation === Orientation.Portrait ? 0 : bar.height
    property int barY: app.orientation === Orientation.Portrait ? Screen.height - bar.height : 0

    readonly property alias barHeight: bar.height

    ControlBar {
        id: bar
        busy: app.fetcherBusyStatus || dm.removerBusy || dm.busy
        rotation: app.orientation === Orientation.Portrait ? 0 : 90
        transformOrigin: Item.TopLeft
        width: app.panelWidth

        onCancelClicked: {
            dm.cancel()
            fetcher.cancel()
            dm.removerCancel()
        }

        onBusyChanged: {
            if (!busy)
                hide()
        }

        y: app.barY
        x: app.barX
    }

    /*Guide {
        id: guide

        rotation: app.orientation === Orientation.Portrait ? 0 : 90
        transformOrigin: Item.TopLeft
        height: app.orientation === Orientation.Portrait ? app.height : app.width
        width: app.orientation === Orientation.Portrait ? app.width : app.height
        y: 0
        x: app.orientation === Orientation.Portrait ? 0 : app.width
    }*/

    Pocket {
        id: pocket
    }
}

