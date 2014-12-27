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

    cover: CoverPage {}

    Component.onCompleted: {
        db.init();
    }

    function resetView() {
        if (!settings.signedIn) {
            pageStack.replaceAbove(null,Qt.resolvedUrl("FirstPage.qml"));
            return;
        }

        utils.setRootModel();
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
            pageStack.replaceAbove(null,Qt.resolvedUrl("EntryPage.qml"));
            break;
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
                settings.reset();
                db.init();
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
            //console.log("DM progress: " + remaining);
            progressPanelDm.text = qsTr("%1 more items left...").arg(remaining);
            if (remaining === 0) {
                progressPanelDm.text = qsTr("All done!");
            }
        }

        onNetworkNotAccessible: {
            notification.show(qsTr("Download failed!\nNetwork connection is unavailable."));
        }

        onRemoverProgressChanged: {
            progressPanelRemover.progress = current / total;
        }
    }

    Connections {
        target: fetcher

        onReady: {
            resetView();

            if (settings.autoDownloadOnUpdate)
                dm.startFeedDownload();
        }

        onNetworkNotAccessible: {
            notification.show(qsTr("Sync failed!\nNetwork connection is unavailable."));
        }

        onError: {
            console.log("Fetcher error");
            console.log("code=" + code);

            if (code < 400)
                return;
            if (code >= 400 && code < 500) {
                if (code == 402)
                    notification.show(qsTr("The user name or password is incorrect!"));
                // Sign in
                pageStack.push(Qt.resolvedUrl("SignInDialog.qml"),{"code": code});
            } else {
                // Unknown error
                notification.show(qsTr("Something went wrong :-(\nAn unknown error occurred."));
            }
        }

        onErrorCheckingCredentials: {
            notification.show(qsTr("The user name or password is incorrect!"));
        }

        onCredentialsValid: {
            notification.show(qsTr("You are signed in!"));
        }

        onProgress: {
            progressPanel.text = qsTr("Receiving data... ");
            progressPanel.progress = current / total;
        }

        onUploading: {
            progressPanel.text = qsTr("Sending data...");
        }

        onBusyChanged: {
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

    }

    Notification {
        id: notification
    }

    property int panelHeightPortrait: 1.1*Theme.itemSizeSmall
    property int panelHeightLandscape: Theme.itemSizeSmall
    property int flickHeight: {
        var size = 0;
        var d = app.orientation==Orientation.Portrait ? app.panelHeightPortrait : app.panelHeightLandscape;
        if (bar.open)
            size += d;
        if (progressPanel.open||progressPanelRemover.open||progressPanelDm.open)
            size += d;
        return app.orientation==Orientation.Portrait ? app.height-size : app.width-size;
    }
    property int panelX: {
        if (app.orientation==Orientation.Portrait)
            return 0;
        if (bar.open)
            return 2*app.panelHeightLandscape;
        return app.panelHeightLandscape;
    }
    property int panelY: {
        if (app.orientation==Orientation.Portrait) {
            if (bar.open)
                return app.height-2*app.panelHeightPortrait;
            return app.height-app.panelHeightPortrait;
        }
        return 0;
    }

    ProgressPanel {
        id: progressPanelRemover
        open: dm.removerBusy
        onCloseClicked: dm.removerCancel();

        rotation: app.orientation==Orientation.Portrait ? 0 : 90
        transformOrigin: Item.TopLeft
        height: app.orientation==Orientation.Portrait ? panelHeightPortrait : panelHeightLandscape
        width: app.orientation==Orientation.Portrait ? app.width : app.height
        y: panelY
        x: panelX
        text: qsTr("Removing cache data...");
        Behavior on y { NumberAnimation { duration: 200;easing.type: Easing.OutQuad } }
        Behavior on x { NumberAnimation { duration: 200;easing.type: Easing.OutQuad } }
    }

    ProgressPanel {
        id: progressPanelDm
        open: dm.busy && !fetcher.busy
        onCloseClicked: dm.cancel();

        rotation: app.orientation==Orientation.Portrait ? 0 : 90
        transformOrigin: Item.TopLeft
        height: app.orientation==Orientation.Portrait ? panelHeightPortrait : panelHeightLandscape
        width: app.orientation==Orientation.Portrait ? app.width : app.height
        y: panelY
        x: panelX
        Behavior on y { NumberAnimation { duration: 200;easing.type: Easing.OutQuad } }
        Behavior on x { NumberAnimation { duration: 200;easing.type: Easing.OutQuad } }
    }

    ProgressPanel {
        id: progressPanel
        open: fetcher.busy
        onCloseClicked: fetcher.cancel();

        rotation: app.orientation==Orientation.Portrait ? 0 : 90
        transformOrigin: Item.TopLeft
        height: app.orientation==Orientation.Portrait ? panelHeightPortrait : panelHeightLandscape
        width: app.orientation==Orientation.Portrait ? app.width : app.height
        y: panelY
        x: panelX
        Behavior on y { NumberAnimation { duration: 200;easing.type: Easing.OutQuad } }
        Behavior on x { NumberAnimation { duration: 200;easing.type: Easing.OutQuad } }
    }

    ControlBar {
        id: bar
        rotation: app.orientation==Orientation.Portrait ? 0 : 90
        transformOrigin: Item.TopLeft
        height: app.orientation==Orientation.Portrait ? panelHeightPortrait : panelHeightLandscape
        width: app.orientation==Orientation.Portrait ? app.width : app.height
        y: app.orientation==Orientation.Portrait ? app.height-height : 0
        x: app.orientation==Orientation.Portrait ? 0 : height

        onOpenChanged: {
            if(open && !settings.helpDone)
                guide.showDelayed();
        }
    }

    Guide {
        id: guide

        rotation: app.orientation==Orientation.Portrait ? 0 : 90
        transformOrigin: Item.TopLeft
        height: app.orientation==Orientation.Portrait ? app.height : app.width
        width: app.orientation==Orientation.Portrait ? app.width : app.height
        y: app.orientation==Orientation.Portrait ? app.height-height : 0
        x: app.orientation==Orientation.Portrait ? 0 : height
    }
}

//fillMode: Image.PreserveAspectFit
//source: "image://theme/graphic-gradient-edge?"+Theme.highlightBackgroundColor
//source: "image://theme/graphic-gradient-home-bottom?"+Theme.highlightBackgroundColor
//source: "image://theme/graphic-gradient-home-top?"+Theme.highlightBackgroundColor

