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
import com.nokia.symbian 1.1

PageStackWindow {
    id: app

    initialPage: mainPage

    showToolBar: !fetcher.busy

    Component.onCompleted: {
        db.init();
    }

    Connections {
        target: settings

        onError: {
            console.log("Settings error!");
            console.log("code=" + code);
            Qt.quit();
        }

        onDashboardInUseChanged: {
            //console.log("onDashboardInUseChanged");
            utils.setTabModel(settings.dashboardInUse);
            pageStack.clear();
            pageStack.replace(Qt.resolvedUrl("TabPage.qml"));
            notification.show(qsTr("Dashboard changed!"));
        }
    }

    Connections {
        target: db

        onError: {
            console.log("DB error!");
            Qt.quit();
        }

        onEmpty: {
            console.log("DB empty");
            dm.removeCache();
            utils.updateModels();
            utils.setTabModel(settings.dashboardInUse);
            pageStack.replace(Qt.resolvedUrl("TabPage.qml"));
        }

        onNotEmpty: {
            console.log("DB not empty");
            utils.setTabModel(settings.dashboardInUse);
            pageStack.replace(Qt.resolvedUrl("TabPage.qml"));
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
    }

    Connections {
        target: fetcher

        onReady: {
            //notification.show(qsTr("Sync done!"));
            utils.setTabModel(settings.dashboardInUse);
            pageStack.replace(Qt.resolvedUrl("TabPage.qml"));

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
            //console.log("Receiving data... " + current / total);
            progressPanel.text = qsTr("Receiving data... ");
            progressPanel.progress = current / total;
        }

        onUploading: {
            //console.log("Sending data to Netvibes...");
            progressPanel.text = qsTr("Sending data to Netvibes...");
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

    ProgressPanel {
        id: progressPanelDm
        open: dm.busy && !fetcher.busy
        onCloseClicked: dm.cancel();

        anchors.bottom: parent.bottom
        anchors.bottomMargin: inPortrait ? privateStyle.toolBarHeightPortrait : privateStyle.toolBarHeightLandscape
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
