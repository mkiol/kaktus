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

    property bool isTablet: Screen.width > 540

    property bool isNetvibes: settings.signinType >= 0 && settings.signinType < 10
    property bool isOldReader: settings.signinType >= 10 && settings.signinType < 20
    property bool isFeedly: settings.signinType >= 20 && settings.signinType < 30

    cover: CoverPage {}

    Component.onCompleted: {
        db.init();
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
            //console.log("settings.signinType:",type);

            if (type < 10)
                reconnectFetcher(1);
            else if (type < 20)
                reconnectFetcher(2);
            else if (type < 30)
                reconnectFetcher(3);
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
                if(!settings.helpDone)
                    guide.showDelayed();
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
                notification.show(qsTr("Something went wrong :-(\nRestart the app to rebuild cache data."));
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
            if (current > 0 && total != 0) {
                progressPanelDm.text = qsTr("Caching... %1 of %2").arg(current).arg(total);
                progressPanelDm.progress = current / total;
                /*if (current == total) {
                    progressPanelDm.text = qsTr("All done!");
                }*/
            } else {
                progressPanelDm.text = qsTr("Caching...");
            }
        }

        onNetworkNotAccessible: {
            notification.show(qsTr("Download failed!\nNetwork connection is unavailable."));
        }

        onRemoverProgressChanged: {
            progressPanelRemover.progress = current / total;
        }

        /*onError: {
            console.log("DM error code:", code);
        }*/
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
        notification.show(qsTr("Something goes wrong. Unable to sign in! :-("));
    }

    function fetcherNetworkNotAccessible() {
        notification.show(qsTr("Sync failed!\nNetwork connection is unavailable."));
    }

    function fetcherError(code) {
        console.log("Fetcher error");
        console.log("code=" + code);

        if (code < 400)
            return;
        if (code >= 400 && code < 500) {
            if (code == 402)
                notification.show(qsTr("The user name or password is incorrect!"));
            /*if (code == 403) {
                notification.show(qsTr("Your login credentials have expired!"));
                if (settings.getSigninType()>0) {
                    fetcher.getAuthUrl();
                    return;
                }
            }*/
            //console.log("settings.signinType",settings.getSigninType());

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
            if (type < 30) {
                pageStack.push(Qt.resolvedUrl("FeedlySignInDialog.qml"),{"code": code});
                return;
            }

        } else {
            // Unknown error
            notification.show(qsTr("Something went wrong :-(\nAn unknown error occurred."));
            resetView();
        }
    }

    function fetcherErrorCheckingCredentials() {
        notification.show(qsTr("The user name or password is incorrect!"));
    }

    function fetcherCredentialsValid() {
        notification.show(qsTr("You are signed in!"));
    }

    function fetcherProgress(current, total) {
        //console.log("fetcherProgress", current, total);
        progressPanel.text = qsTr("Receiving data... ");
        progressPanel.progress = current / total;
    }

    function fetcherUploadProgress(current, total) {
        //console.log("fetcherUploadProgress", current, total);
        progressPanel.text = qsTr("Sending data...");
        progressPanel.progress = current / total;
    }

    function fetcherUploading() {
        //console.log("fetcherUploading");
        progressPanel.text = qsTr("Sending data...");
    }

    function fetcherBusyChanged() {

        if (app.fetcherBusyStatus != fetcher.busy)
            app.fetcherBusyStatus = fetcher.busy;

        switch(fetcher.busyType) {
        case 1:
            progressPanel.text = qsTr("Initiating...");
            progressPanel.progress = 0;
            break;
        case 2:
            progressPanel.text = qsTr("Updating...");
            progressPanel.progress = 0;
            break;
        case 3:
            progressPanel.text = qsTr("Signing in...");
            progressPanel.progress = 0;
            break;
        case 4:
            progressPanel.text = qsTr("Signing in...");
            progressPanel.progress = 0;
            break;
        case 11:
            progressPanel.text = qsTr("Waiting for network...");
            progressPanel.progress = 0;
            break;
        case 21:
            progressPanel.text = qsTr("Waiting for network...");
            progressPanel.progress = 0;
            break;
        case 31:
            progressPanel.text = qsTr("Waiting for network...");
            progressPanel.progress = 0;
            break;
        }
    }

    function fetcherCanceled() {
        //notification.show(qsTr("Syncing canceled!"));
        resetView();
    }

    Notification {
        id: notification
    }

    /*property int panelHeightPortrait: 1.0*Theme.itemSizeSmall
    property int panelHeightLandscape: 0.9*Theme.itemSizeSmall
    property int barHeightPortrait: 1.1*Theme.itemSizeSmall
    property int barHeightLandscape: 1.0*Theme.itemSizeSmall
    property int panelHeight: app.orientation==Orientation.Portrait ? app.panelHeightPortrait : app.panelHeightLandscape
    property int panelWidth: app.orientation==Orientation.Portrait ? app.width : app.height
    property int barHeight: app.orientation==Orientation.Portrait ? app.barHeightPortrait : app.barHeightLandscape
    property int barWidth: app.orientation==Orientation.Portrait ? app.width : app.height
    property int flickHeight: {
        var size = 0;
        if (bar.open)
            size += barHeight;
        if (progressPanel.open||progressPanelRemover.open||progressPanelDm.open)
            size += panelHeight;
        return app.orientation==Orientation.Portrait ? app.height-size : app.width-size;
    }
    property int barX: {
        if (app.orientation==Orientation.Portrait)
            return 0;
        if (bar.open && (progressPanel.open||progressPanelRemover.open||progressPanelDm.open))
            return app.barHeightLandscape + app.panelHeightLandscape;
        return app.barHeightLandscape;
    }
    property int barY: {
        if (app.orientation==Orientation.Portrait) {
            if (bar.open && (progressPanel.open||progressPanelRemover.open||progressPanelDm.open))
                return app.height - (app.barHeightPortrait + app.panelHeightPortrait);
            return app.height - app.barHeightPortrait;
        }
        return 0;
    }
    property int panelX: app.orientation==Orientation.Portrait ? 0 : app.panelHeight
    property int panelY: app.orientation==Orientation.Portrait ? app.height-app.panelHeight : 0*/

    property int panelHeightPortrait: 1.0*Theme.itemSizeSmall
    property int panelHeightLandscape: 0.9*Theme.itemSizeSmall
    property int barHeightPortrait: 1.1*Theme.itemSizeSmall
    property int barHeightLandscape: 1.0*Theme.itemSizeSmall
    property int panelHeight: app.orientation==Orientation.Portrait ? app.panelHeightPortrait : app.panelHeightLandscape
    property int panelWidth: app.orientation==Orientation.Portrait ? Screen.width : Screen.height
    property int barHeight: app.orientation==Orientation.Portrait ? app.barHeightPortrait : app.barHeightLandscape
    property int barWidth: app.orientation==Orientation.Portrait ? Screen.width : Screen.height
    //property int landscapeContentPanelWidth: app.orientation==Orientation.Portrait ? Screen.width/2 : Screen.height/2
    property int landscapeContentPanelWidth: isTablet ?
                                                 app.orientation==Orientation.Portrait ? Screen.width-700 : Screen.height-700 :
                                                 app.orientation==Orientation.Portrait ? Screen.width/2 : Screen.height/2
    property int flickHeight: {
        var size = 0;
        if (bar.open)
            size += barHeight;
        if (progressPanel.open||progressPanelRemover.open||progressPanelDm.open)
            size += panelHeight;
        return app.orientation==Orientation.Portrait ? Screen.height-size : Screen.width-size;
    }
    property int barX: {
        if (app.orientation==Orientation.Portrait)
            return 0;
        if (bar.open && (progressPanel.open||progressPanelRemover.open||progressPanelDm.open))
            return app.barHeightLandscape + app.panelHeightLandscape;
        return app.barHeightLandscape;
    }
    property int barY: {
        if (app.orientation==Orientation.Portrait) {
            if (bar.open && (progressPanel.open||progressPanelRemover.open||progressPanelDm.open))
                return Screen.height - (app.barHeightPortrait + app.panelHeightPortrait);
            return Screen.height - app.barHeightPortrait;
        }
        return 0;
    }
    property int panelX: app.orientation==Orientation.Portrait ? 0 : app.panelHeight
    property int panelY: app.orientation==Orientation.Portrait ? Screen.height-app.panelHeight : 0

    ControlBar {
        id: bar
        rotation: app.orientation==Orientation.Portrait ? 0 : 90
        transformOrigin: Item.TopLeft
        height: app.barHeight
        width: app.barWidth
        y: app.barY
        x: app.barX
    }

    ProgressPanel {
        id: progressPanelRemover
        open: dm.removerBusy
        onCloseClicked: dm.removerCancel();
        transparent: false

        rotation: app.orientation==Orientation.Portrait ? 0 : 90
        transformOrigin: Item.TopLeft
        height: app.panelHeight
        width: app.panelWidth
        y: app.panelY
        x: app.panelX
        text: qsTr("Removing cache data...");
        Behavior on y { NumberAnimation { duration: 200;easing.type: Easing.OutQuad } }
        Behavior on x { NumberAnimation { duration: 200;easing.type: Easing.OutQuad } }
    }

    ProgressPanel {
        id: progressPanelDm
        open: dm.busy && !app.fetcherBusyStatus
        onCloseClicked: dm.cancel();
        transparent: false

        rotation: app.orientation==Orientation.Portrait ? 0 : 90
        transformOrigin: Item.TopLeft
        height: app.panelHeight
        width: app.panelWidth
        y: app.panelY
        x: app.panelX
        Behavior on y { NumberAnimation { duration: 200;easing.type: Easing.OutQuad } }
        Behavior on x { NumberAnimation { duration: 200;easing.type: Easing.OutQuad } }
    }

    ProgressPanel {
        id: progressPanel
        open: app.fetcherBusyStatus
        onCloseClicked: fetcher.cancel();
        transparent: false

        rotation: app.orientation==Orientation.Portrait ? 0 : 90
        transformOrigin: Item.TopLeft
        height: app.panelHeight
        width: app.panelWidth
        y: app.panelY
        x: app.panelX
        Behavior on y { NumberAnimation { duration: 200;easing.type: Easing.OutQuad } }
        Behavior on x { NumberAnimation { duration: 200;easing.type: Easing.OutQuad } }
    }

    Guide {
        id: guide

        rotation: app.orientation==Orientation.Portrait ? 0 : 90
        transformOrigin: Item.TopLeft
        height: app.orientation==Orientation.Portrait ? app.height : app.width
        width: app.orientation==Orientation.Portrait ? app.width : app.height
        y: 0
        x: app.orientation==Orientation.Portrait ? 0 : app.width
    }

}

