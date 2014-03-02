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
import QtWebKit.experimental 1.0

Page {
    id: root

    property string title
    property string entryId
    property string offlineUrl
    property string onlineUrl
    property bool stared
    property int index

    onForwardNavigationChanged: {
        if (forwardNavigation)
            forwardNavigation = false;
    }
    onBackNavigationChanged: {
        if (backNavigation)
            backNavigation = false;
    }
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

        url:  offLineMode ? offlineUrl : onlineUrl
        experimental.userAgent: settings.getDmUserAgent()

        onUrlChanged: {
            console.log("onUrlChanged");
        }

        /*header: PageHeader {
            title: root.title
        }*/

        onLoadingChanged: {
            if (loadRequest.status == WebView.LoadStartedStatus) {
                console.log("onLoadingChanged, LoadStartedStatus");
                busy.show("Loading page content...", false);
            } else if (loadRequest.status == WebView.LoadFailedStatus) {
                console.log("onLoadingChanged, LoadFailedStatus");
                notification.show(qsTr("Failed to load page content :-("));
                busy.hide();
            } else {
                console.log("onLoadingChanged, Ok");
                busy.hide();
            }
        }

        onNavigationRequested: {
            // In Off-Line mode navigation is disabled
            if (offLineMode) {
                if (request.url != offlineUrl) {
                    request.action = WebView.IgnoreRequest;
                }
            }
        }
    }

    ControlBar {
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
            if (offLineMode)
                Qt.openUrlExternally(offlineUrl);
            else
                Qt.openUrlExternally(onlineUrl);
        }
    }

    BusyBar {
        id: busy
        onCloseClicked: {
            console.log("cancel!");
        }
    }

}
