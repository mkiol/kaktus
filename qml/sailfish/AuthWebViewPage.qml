/*
  Copyright (C) 2015 Michal Kosciesza <michal@mkiol.net>

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

Page {
    id: root

    property bool showBar: false

    property string url
    property int type
    property int code

    ActiveDetector {}

    function accept() {

        var doInit = settings.signinType != type;
        settings.signinType = type;

        if (code == 0) {
            fetcher.checkCredentials();
        } else {
            if (! dm.busy)
                dm.cancel();
            if (doInit)
                fetcher.init();
            else
                fetcher.update();
        }

        if (pageStack.depth==3) {
            pageStack.replaceAbove(null,Qt.resolvedUrl("FirstPage.qml"));
        } else {
            pageStack.pop();
        }
    }

    onForwardNavigationChanged: {
        if (forwardNavigation)
            forwardNavigation = false;
    }

    /*onBackNavigationChanged: {
        if (backNavigation)
            backNavigation = false;
    }*/

    showNavigationIndicator: false

    allowedOrientations: {
        switch (settings.allowedOrientations) {
        case 1:
            return Orientation.Portrait;
        case 2:
            return Orientation.Landscape;
        }
        return Orientation.Landscape | Orientation.Portrait;
    }

    SilicaWebView {
        id: view

        //anchors { top: parent.top; left: parent.left; right: parent.right; bottom: parent.bottom }
        anchors {left: parent.left; right: parent.right}
        height: controlbar.open ? parent.height - controlbar.height : parent.height
        url: root.url
        clip: true

        _cookiesEnabled: false
        experimental.userAgent: settings.getDmUserAgent()
        experimental.preferences.offlineWebApplicationCacheEnabled: false
        experimental.preferences.localStorageEnabled: false
        experimental.preferences.privateBrowsingEnabled: true

        onLoadingChanged: {
            switch (loadRequest.status) {
            case WebView.LoadStartedStatus:
                proggressPanel.text = qsTr("Loading page content...");
                proggressPanel.open = true;
                break;
            case WebView.LoadSucceededStatus:
                proggressPanel.open = false;
                break;
            case WebView.LoadFailedStatus:
                proggressPanel.open = false;
                notification.show(qsTr("Failed to load page content :-("));
                break;
            default:
                proggressPanel.open = false;
            }
        }

        onNavigationRequested: {
            if (!Qt.application.active) {
                request.action = WebView.IgnoreRequest;
            }
        }

        onUrlChanged: {
            //console.log("Url changed:", url);
            if (fetcher.setConnectUrl(url)) {
                accept();
            }
        }
    }

    ControlBarWebPreview {
        id: controlbar
        flick: view
        canBack: true
        canStar: false
        canOpenBrowser: false
        canReader: false
        stared: false
        transparent: false
        onBackClicked: pageStack.pop()
    }

    ProgressPanel {
        id: proggressPanel
        transparent: false
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        height: isPortrait ? app.panelHeightPortrait : app.panelHeightLandscape
        cancelable: true
        onCloseClicked: view.stop()
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
