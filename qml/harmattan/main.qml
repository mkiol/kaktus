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

import QtQuick 1.1
import com.nokia.meego 1.0

import "Theme.js" as Theme

PageStackWindow {
    id: app

    initialPage: mainPage

    showToolBar: !fetcher.busy

    Component.onCompleted: {
        theme.inverted = true;
        db.init();
    }

    function resetView() {
        if (!settings.signedIn) {
            //pageStack.clear();
            pageStack.push(null,Qt.resolvedUrl("FirstPage.qml"));
            return;
        }

        utils.setRootModel();
        switch (settings.viewMode) {
        case 0:
        case 1:
            //pageStack.clear();
            pageStack.push(null,Qt.resolvedUrl("TabPage.qml"));
            break;
        case 2:
            //pageStack.clear();
            pageStack.push(null,Qt.resolvedUrl("FeedPage.qml"),{"title": qsTr("Feeds")});
            break;
        case 3:
        case 4:
        case 5:
            //pageStack.clear();
            pageStack.push(null,Qt.resolvedUrl("EntryPage.qml"));
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
        }

        onViewModeChanged: {
            resetView();
        }

        onSignedInChanged: {
            if (!settings.signedIn) {
                notification.show(qsTr("Signed out!"));
                fetcher.cancel(); dm.cancel();
                settings.reset();
                db.init();
            }
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

            if (settings.getAutoDownloadOnUpdate())
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
                notification.show(qsTr("An unknown error occurred! :-("));
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

    MainPage {
        id: mainPage
    }

    Notification {
        id: notification
    }

    property bool isPortrait: screen.currentOrientation==Screen.Portrait || screen.currentOrientation==Screen.PortraitInverted

    ProgressPanel {
        id: progressPanelRemover
        open: dm.removerBusy
        onCloseClicked: dm.removerCancel();

        anchors.bottom: parent.bottom
        anchors.bottomMargin: isPortrait ? Theme.navigationBarPortrait : Theme.navigationBarLanscape
        anchors.right: parent.right
        anchors.left: parent.left
    }

    ProgressPanel {
        id: progressPanelDm
        open: dm.busy && !fetcher.busy
        onCloseClicked: dm.cancel();

        anchors.bottom: parent.bottom
        anchors.bottomMargin: isPortrait ? Theme.navigationBarPortrait : Theme.navigationBarLanscape
        anchors.right: parent.right
        anchors.left: parent.left
    }

    ProgressPanel {
        id: progressPanel
        open: fetcher.busy
        onCloseClicked: fetcher.cancel();

        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.left: parent.left
    }

    Menu {
        id: menu
        visualParent: pageStack
        MenuLayout {
            MenuItem {
                text: qsTr("About")
                onClicked: pageStack.push(Qt.resolvedUrl("AboutPage.qml"));
            }
            MenuItem {
                text: qsTr("Settings")
                onClicked: pageStack.push(Qt.resolvedUrl("SettingsPage.qml"));
            }
            MenuItem {
                text: qsTr("Exit")
                onClicked: Qt.quit()
            }
        }
    }

    Menu {
        id: simpleMenu
        visualParent: pageStack
        MenuLayout {
            MenuItem {
                text: qsTr("Exit")
                onClicked: Qt.quit()
            }
        }
    }
}
