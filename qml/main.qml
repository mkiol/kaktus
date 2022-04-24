/* Copyright (C) 2014-2022 Michal Kosciesza <michal@mkiol.net>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

import QtQuick 2.0
import Sailfish.Silica 1.0
import QtQuick.Window 2.0

import harbour.kaktus.Settings 1.0

ApplicationWindow {
    id: app

    property bool progress: false
    property var oldViewMode

    readonly property bool isTablet: Screen.sizeCategory === Screen.Large || Screen.sizeCategory === Screen.ExtraLarge
    readonly property bool isNetvibes: settings.signinType >= 0 && settings.signinType < 10
    readonly property bool isOldReader: settings.signinType >= 10 && settings.signinType < 20
    readonly property bool isTTRss: settings.signinType >= 30 && settings.signinType < 40
    readonly property int stdHeight: isPortraitOrientation(orientation) ? Theme.itemSizeMedium : 0.8 * Theme.itemSizeMedium

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

    function isPortraitOrientation(orientation) {
        return orientation === Orientation.Portrait || orientation === Orientation.PortraitInverted;
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
        if ((oldViewMode === Settings.AllEntries || oldViewMode === Settings.SavedEntries ||
             oldViewMode === Settings.SlowEntries || oldViewMode === Settings.LikedEntries ||
             oldViewMode === Settings.BroadcastedEntries) &&
                (newViewMode === Settings.AllEntries || newViewMode === Settings.SavedEntries ||
                 newViewMode === Settings.SlowEntries || newViewMode === Settings.LikedEntries ||
                 newViewMode === Settings.BroadcastedEntries)) {
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
            console.log("Settings error: code=" + code);
            Qt.quit();
        }

        onDashboardInUseChanged: {
            resetView();
        }

        onViewModeChanged: {
            resetView();
        }

        onSignedInChanged: {
            if (!settings.signedIn) {
                fetcher.cancel(); dm.cancel();
                db.init();
            }
        }

        onShowBroadcastChanged: {
            if (!settings.showBroadcast && settings.viewMode === Settings.LikedEntries) {
                settings.viewMode = Settings.SavedEntries;
            }
        }
    }

    Connections {
        target: db

        onError: {
            console.log("DB error: code="+code);
            if (code == 511) {
                notification.show(qsTr("Restart the app to rebuild cache data"));
                return;
            }
            Qt.quit();
        }

        onEmpty: {
            dm.removeCache();
            if (settings.viewMode !== Settings.TabsFeedsEntries)
                settings.viewMode = Settings.TabsFeedsEntries;
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
                        settings.offlineMode = false
                    }
                } else {
                    if (!settings.offlineMode) {
                        settings.offlineMode = true
                    }
                }
            }
        }

        onNetworkNotAccessible: {
            notification.show(qsTr("Download has failed because network is disconnected"));
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
        fetcher.imageSaved.connect(fetcherImageSaved);
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
        fetcher.imageSaved.disconnect(fetcherImageSaved);
    }

    property bool fetcherBusyStatus: false

    function fetcherReady() {
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
        notification.show(qsTr("Unable to sign in"));
    }

    function fetcherNetworkNotAccessible() {
        notification.show(qsTr("Network connection is unavailable"));
    }

    function fetcherError(code) {
        console.log("Fetcher error: code=" + code);

        if (code < 400)
            return;
        if (code === 700 || (code >= 400 && code < 500)) {
            if (code === 402)
                notification.show(qsTr("The user name or password is incorrect"));
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
        } else if (code === 800) {
            notification.show(qsTr("Cannot save image in gallery"));
        } else if (code === 801) {
            notification.show(qsTr("Image already exists in gallery"));
        } else {
            // Unknown error
            notification.show(qsTr("Unknown error"));
            resetView();
        }
    }

    function fetcherErrorCheckingCredentials() {
        notification.show(qsTr("The user name or password is incorrect"));
    }

    function fetcherCredentialsValid() {
        notification.show(qsTr("You are signed in"));
    }

    function fetcherProgress(current, total) {
        //console.log("fetcherProgress", current, total);
        bar.progressText = qsTr("Receiving data...");
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

    function fetcherImageSaved(filename) {
        notification.show(qsTr("Image saved in gallery as \"" + filename + "\""));
    }

    Notification {
        id: notification
    }

    property int panelWidth: isPortraitOrientation(app.orientation) ? Screen.width : Screen.height
    property int landscapeContentPanelWidth: isTablet ?
                                                 isPortraitOrientation(app.orientation) ? Screen.width-700 : Screen.height-700 :
    isPortraitOrientation(app.orientation) ? Screen.width/2 : Screen.height/2
    property int flickHeight: {
        var size = 0
        if (bar.open)
            size += bar.stdHeight
        return isPortraitOrientation(app.orientation) ? Screen.height-size : Screen.width-size;
    }

    property int barX: {
        switch (app.orientation) {
        case Orientation.Portrait: return 0;
        case Orientation.Landscape: return bar.height;
        case Orientation.PortraitInverted: return Screen.width;
        case Orientation.LandscapeInverted: return Screen.width - bar.height;
        }
    }

    property int barY: {
        switch (app.orientation) {
        case Orientation.Portrait: return Screen.height - bar.height;
        case Orientation.Landscape: return 0;
        case Orientation.PortraitInverted: return bar.height;
        case Orientation.LandscapeInverted: return Screen.height;
        }
    }

    readonly property alias barHeight: bar.height

    ControlBar {
        id: bar
        busy: app.fetcherBusyStatus || dm.removerBusy || dm.busy

        rotation: {
            switch (app.orientation) {
            case Orientation.Portrait: return 0;
            case Orientation.Landscape: return 90;
            case Orientation.PortraitInverted: return 180;
            case Orientation.LandscapeInverted: return 270;
            }
        }

        transformOrigin: Item.TopLeft
        width: app.panelWidth

        onCancelClicked: {
            dm.cancel()
            fetcher.cancel()
            dm.removerCancel()
        }

        onBusyChanged: {
            if (!busy) hide()
        }

        y: app.barY
        x: app.barX
    }

    Pocket {
        id: pocket
    }
}

