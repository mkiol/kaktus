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
import QtWebKit 1.0

import "Theme.js" as Theme

Page {
    id: root

    property string title
    property string entryId
    property string offlineUrl
    property string onlineUrl
    property bool stared
    property bool read
    property int index
    property int feedindex
    property bool cached
    property int markAsReadTime: 4000

    property bool isPortrait: screen.currentOrientation==Screen.Portrait || screen.currentOrientation==Screen.PortraitInverted

    ActiveDetector {}

    tools:  WebToolbar {
        stared: root.stared

        onStarClicked: {
            if (stared) {
                stared=false;
                entryModel.setData(root.index, "readlater", 0);
            } else {
                stared=true;
                entryModel.setData(root.index, "readlater", 1);
            }
        }

        onBrowserClicked: {
            notification.show(qsTr("Launching an external browser..."));
            Qt.openUrlExternally(onlineUrl);
        }

        onOfflineClicked: {
            if (settings.offlineMode) {
                if (dm.online)
                    settings.offlineMode = false;
                else
                    notification.show(qsTr("Cannot switch to Online mode\nNetwork connection is unavailable"));
            } else {
                if (root.cached)
                    settings.offlineMode = true;
                else
                    notification.show(qsTr("Offline version not available"));
            }
        }
    }

    orientationLock: {
        switch (settings.allowedOrientations) {
        case 1:
            return PageOrientation.LockPortrait;
        case 2:
            return PageOrientation.LockLandscape;
        }
        return PageOrientation.Automatic;
    }

    FlickableWebView {
        id: view

        url:  {
            if (settings.offlineMode) {
                //console.log("offline");
                if (isPortrait)
                    return offlineUrl+"?width=480px";
                return offlineUrl+"?width=746px";
            }
            return onlineUrl;
        }

        onProgressChanged: {
            //console.log("progress:"+progress);

            proggressPanel.progress = progress;

            if (progress<1) {
                proggressPanel.text = qsTr("Loading page content...");
                proggressPanel.open = true;
            } else {
                proggressPanel.open = false;

                // Start timer to mark as read
                if (!root.read)
                    timer.start();
            }
        }

        onLoadFailed: console.log("LoadFailed")
        onLoadFinished: console.log("LoadFinished")
        onLoadStarted: console.log("LoadStarted")

        anchors.fill: parent
    }

    ProgressPanel {
        id: proggressPanel
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        cancelable: true
        onCloseClicked: view.stop()
    }

    Timer {
        id: timer
        interval: root.markAsReadTime
        onTriggered: {
            if (!root.read) {
                read=true;
                entryModel.setData(root.index, "read", 1);
            }
        }
    }

    Connections {
        target: fetcher
        onBusyChanged: pageStack.pop()
    }

    Connections {
        target: dm
        onBusyChanged: pageStack.pop()
    }

    // Workaround for 'High Power Consumption' webkit bug
    Connections {
        target: Qt.application
        onActiveChanged: {
            if(!Qt.application.active && settings.powerSaveMode) {
                pageStack.pop();
            }
        }
    }
}
