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
import QtWebKit 3.0
//import QtWebKit.experimental 1.0 //Not allowed in harbour :-(

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
    property int markAsReadTime: 4000

    onForwardNavigationChanged: {
        if (forwardNavigation)
            forwardNavigation = false;
    }

    /*onBackNavigationChanged: {
        if (backNavigation)
            backNavigation = false;
    }*/

    showNavigationIndicator: false
    //allowedOrientations: Orientation.Landscape | Orientation.Portrait
    allowedOrientations: Orientation.Portrait

    SilicaWebView {
        id: view

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        url:  settings.offlineMode ? offlineUrl : onlineUrl
        //experimental.userAgent: settings.getDmUserAgent()

        onLoadingChanged: {
            if (loadRequest.status == WebView.LoadStartedStatus) {
                busy.show(qsTr("Loading page content..."), true);
            } else if (loadRequest.status == WebView.LoadFailedStatus) {
                if (settings.offlineMode)
                    notification.show(qsTr("Failed to load article from local cache :-("));
                else
                    notification.show(qsTr("Failed to load page content :-("));
                busy.hide();
            } else {
                busy.hide();

                // Start timer to mark as read
                if (!root.read && settings.getAutoMarkAsRead())
                    timer.start();
            }
        }

        onNavigationRequested: {
            // In Off-Line mode navigation is disabled
            if (settings.offlineMode) {
                if (request.url != offlineUrl) {
                    request.action = WebView.IgnoreRequest;
                }
            }
        }
    }

    ControlBarWebPreview {
        id: controlbar
        flick: view
        canBack: true
        canStar: true
        canOpenBrowser: true
        stared: root.stared

        onBackClicked: pageStack.pop()

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
            if (settings.offlineMode)
                Qt.openUrlExternally(offlineUrl);
            else
                Qt.openUrlExternally(onlineUrl);
        }
    }

    BusyBar {
        id: busy
        cancelable: true
        onCloseClicked: view.stop()
    }

    Timer {
        id: timer
        interval: root.markAsReadTime
        onTriggered: {
            if (!root.read && settings.getAutoMarkAsRead()) {
                read=true;
                entryModel.setData(root.index, "read", 1);
                feedModel.decrementUnread(feedindex);
                //notification.show(qsTr("Marked as read!"));
            }
        }
    }
}
