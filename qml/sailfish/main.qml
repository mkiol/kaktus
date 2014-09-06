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
        utils.setRootModel();
        switch (settings.viewMode) {
        case 0:
            pageStack.replaceAbove(null,Qt.resolvedUrl("TabPage.qml"));
            break;
        case 1:
            pageStack.replaceAbove(null,Qt.resolvedUrl("TabPage.qml"));
            break;
        case 2:
            pageStack.replaceAbove(null,Qt.resolvedUrl("FeedPage.qml"),{"title": qsTr("Feeds")});
            break;
        case 3:
            pageStack.replaceAbove(null,Qt.resolvedUrl("EntryPage.qml"));
            break;
        }
    }

    Connections {
        target: settings

        onError: {
            console.log("Settings error!");
            console.log("code=" + code);
            Qt.quit();
        }

        onDashboardInUseChanged: {
            resetView();
            notification.show(qsTr("Dashboard changed!"));
        }

        onViewModeChanged: {
            resetView();
            //notification.show(qsTr("Browsing mode changed!"));
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
            //utils.updateModels();
            //utils.setTabModel(settings.dashboardInUse);
            //pageStack.replaceAbove(null,Qt.resolvedUrl("TabPage.qml"));
            resetView();
        }

        onNotEmpty: {
            //utils.setTabModel(settings.dashboardInUse);
            //pageStack.replaceAbove(null,Qt.resolvedUrl("TabPage.qml"));
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
            notification.show(qsTr("Download failed\nNetwork connection is unavailable"));
        }

        onBusyChanged: {
            if (dm.busy && bar.open)
                bar.open = false;
        }

        onRemoverBusyChanged: {
            if (dm.removerBusy && bar.open)
                bar.open = false;
        }

        onRemoverProgressChanged: {
            //console.log("Remover progress: " + current / total);
            progressPanelRemover.progress = current / total;
        }
    }

    Connections {
        target: fetcher

        onReady: {
            //notification.show(qsTr("Sync done!"));
            resetView();

            if (settings.getAutoDownloadOnUpdate())
                dm.startFeedDownload();
        }

        onNetworkNotAccessible: {
            notification.show(qsTr("Sync failed\nNetwork connection is unavailable"));
        }

        onError: {
            console.log("Fetcher error");
            console.log("code=" + code);

            if (code < 400)
                return;
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
        }

        onCredentialsValid: {
            notification.show(qsTr("Successfully Signed In!"));
        }

        onProgress: {
            progressPanel.text = qsTr("Receiving data... ");
            progressPanel.progress = current / total;
        }

        onUploading: {
            progressPanel.text = qsTr("Sending data to Netvibes...");
        }

        onBusyChanged: {

            if (fetcher.busy && bar.open)
                bar.open = false;

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

    ControlBar {
        id: bar
        open: false

        rotation: app.orientation==Orientation.Portrait ? 0 : 90
        transformOrigin: Item.TopLeft
        height: app.orientation==Orientation.Portrait ? Theme.itemSizeMedium : 0.8*Theme.itemSizeMedium
        width: app.orientation==Orientation.Portrait ? app.width : app.height
        y: app.orientation==Orientation.Portrait ? app.height-height : 0
        x: app.orientation==Orientation.Portrait ? 0 : height
    }

    ProgressPanel {
        id: progressPanelRemover
        open: dm.removerBusy
        onCloseClicked: dm.removerCancel();

        rotation: app.orientation==Orientation.Portrait ? 0 : 90
        transformOrigin: Item.TopLeft
        height: app.orientation==Orientation.Portrait ? Theme.itemSizeMedium : 0.8*Theme.itemSizeMedium
        width: app.orientation==Orientation.Portrait ? app.width : app.height
        y: app.orientation==Orientation.Portrait ? app.height-height : 0
        x: app.orientation==Orientation.Portrait ? 0 : height
        text: qsTr("Removing cache data...");
    }

    ProgressPanel {
        id: progressPanelDm
        open: dm.busy && !fetcher.busy
        onCloseClicked: dm.cancel();

        rotation: app.orientation==Orientation.Portrait ? 0 : 90
        transformOrigin: Item.TopLeft
        height: app.orientation==Orientation.Portrait ? Theme.itemSizeMedium : 0.8*Theme.itemSizeMedium
        width: app.orientation==Orientation.Portrait ? app.width : app.height
        y: app.orientation==Orientation.Portrait ? app.height-height : 0
        x: app.orientation==Orientation.Portrait ? 0 : height
    }

    ProgressPanel {
        id: progressPanel
        open: fetcher.busy
        //open: true
        onCloseClicked: fetcher.cancel();

        rotation: app.orientation==Orientation.Portrait ? 0 : 90
        transformOrigin: Item.TopLeft
        height: app.orientation==Orientation.Portrait ? Theme.itemSizeMedium : 0.8*Theme.itemSizeMedium
        width: app.orientation==Orientation.Portrait ? app.width : app.height
        y: app.orientation==Orientation.Portrait ? app.height-height : 0
        x: app.orientation==Orientation.Portrait ? 0 : height
    }

}

//fillMode: Image.PreserveAspectFit
//source: "image://theme/graphic-gradient-edge?"+Theme.highlightBackgroundColor
//source: "image://theme/graphic-gradient-home-bottom?"+Theme.highlightBackgroundColor
//source: "image://theme/graphic-gradient-home-top?"+Theme.highlightBackgroundColor

