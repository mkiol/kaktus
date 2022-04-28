/* Copyright (C) 2017-2022 Michal Kosciesza <michal@mkiol.net>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

import QtQuick 2.1
import Sailfish.Silica 1.0
import Sailfish.WebView 1.0

WebViewPage {
    id: root

    property bool showBar: false
    property bool _done: false
    property bool _doPop: false
    readonly property color _bgColor: Theme.colorScheme === Theme.LightOnDark ?
                                          Qt.darker(Theme.highlightBackgroundColor, 5.0) :
                                          Qt.lighter(Theme.highlightBackgroundColor, 1.8)

    function init() {
        view.url = pocket.getAuthUrl()
    }

    function navigateBack() {
        if (view.canGoBack) {
            view.goBack()
        } else {
            pageStack.pop()
        }
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
            return Orientation.PortraitMask;
        case 2:
            return Orientation.LandscapeMask;
        }
        return Orientation.All;
    }

    onStatusChanged: {
        if (status === PageStatus.Active && root._doPop) {
            pageStack.pop()
        }
    }

    WebView {
        id: view

        anchors.fill: parent
        canShowSelectionMarkers: true

        onUrlChanged: {
            var surl = url.toString()
            console.log("url changed:", surl)

            if (surl === "https://getpocket.com/a/") {
                init()
                return
            }

            if (surl === "https://localhost/kaktusAuthorizationFinished") {
                root._done = true
                pocket.check()
                if (status === PageStatus.Active) {
                    pageStack.pop()
                } else {
                    root._doPop = true
                }
            }
        }
    }

    IconBar {
        id: controlbar
        flickable: view
        color: root._bgColor

        IconBarItem {
            text: qsTr("Back")
            icon: "image://theme/icon-m-back"
            onClicked: root.navigateBack()
        }
    }

    Component.onDestruction: {
        if (!root._done) {
            pocket.cancel()
        }
    }
}
