/*
  Copyright (C) 2017 Michal Kosciesza <michal@mkiol.net>

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

import QtQuick 2.1
import Sailfish.Silica 1.0
import QtWebKit 3.0

Page {
    id: root

    property bool showBar: false
    property bool doPop: false
    property bool done: false

    function init() {
        view.url = pocket.getAuthUrl()
    }

    Component.onCompleted: {
        init()
    }

    onForwardNavigationChanged: {
        if (forwardNavigation)
            forwardNavigation = false
    }

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

    onStatusChanged: {
        if (status === PageStatus.Active && doPop) {
            pageStack.pop()
        }
    }

    SilicaWebView {
        id: view

        anchors {left: parent.left; right: parent.right}
        height: controlbar.open ? parent.height - controlbar.height : parent.height
        clip: true

        _cookiesEnabled: true
        experimental.preferences.offlineWebApplicationCacheEnabled: false
        experimental.preferences.localStorageEnabled: true
        //experimental.preferences.privateBrowsingEnabled: true

        onLoadingChanged: {
            switch (loadRequest.status) {
            case WebView.LoadStartedStatus:
                proggressPanel.text = qsTr("Loading page content...");
                proggressPanel.open = true
                break;
            case WebView.LoadSucceededStatus:
                proggressPanel.open = false
                break;
            case WebView.LoadFailedStatus:
                proggressPanel.open = false
                break;
            default:
                proggressPanel.open = false
            }
        }

        onNavigationRequested: {
            if (!Qt.application.active) {
                request.action = WebView.IgnoreRequest
            }
        }

        onUrlChanged: {
            console.log("Url changed:", url)

            var surl = url.toString()
            if (surl === "https://getpocket.com/a/") {
                init()
                return
            }

            if (surl === "kaktus:authorizationFinished") {
                done = true
                pocket.check()
                if (status === PageStatus.Active) {
                    pageStack.pop()
                } else {
                    doPop = true
                }
            }
        }
    }

    IconBar {
        id: controlbar
        flickable: view
        IconBarItem {
            text: qsTr("Back")
            icon: "image://theme/icon-m-back"
            onClicked: view.canGoBack ? view.goBack() : pageStack.pop()
        }
    }

    ProgressPanel {
        id: proggressPanel
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        cancelable: true
        onCloseClicked: view.stop()
    }

    // Workaround for 'High Power Consumption' webkit bug
    Connections {
        target: Qt.application
        onActiveChanged: {
            if(!Qt.application.active && settings.powerSaveMode) {
                pageStack.pop()
            }
        }
    }

    Component.onDestruction: {
        if (!done) {
            pocket.cancel()
        }
    }
}
