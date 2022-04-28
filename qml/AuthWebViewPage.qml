/* Copyright (C) 2015-2022 Michal Kosciesza <michal@mkiol.net>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.WebView 1.0

WebViewPage {
    id: root

    property bool showBar: false

    property string url
    property int type
    property int code
    readonly property color _bgColor: Theme.colorScheme === Theme.LightOnDark ?
                                          Qt.darker(Theme.highlightBackgroundColor, 5.0) :
                                          Qt.lighter(Theme.highlightBackgroundColor, 1.8)

    ActiveDetector {}

    function navigateBack() {
        if (view.canGoBack) {
            view.goBack()
        } else {
            pageStack.pop()
        }
    }

    function accept() {
        var doInit = settings.signinType != type;
        settings.signinType = type;

        if (code == 0) {
            fetcher.checkCredentials();
        } else {
            if (dm.busy)
                dm.cancel();
            if (doInit)
                fetcher.init();
            else
                fetcher.update();
        }

        if (pageStack.depth===3) {
            pageStack.replaceAbove(null,Qt.resolvedUrl("FirstPage.qml"));
        } else {
            pageStack.pop();
        }
    }

    onForwardNavigationChanged: {
        if (forwardNavigation)
            forwardNavigation = false;
    }

    showNavigationIndicator: false

    allowedOrientations: {
        switch (settings.allowedOrientations) {
        case 1:
            return Orientation.PortraitMask
        case 2:
            return Orientation.LandscapeMask
        }
        return Orientation.All
    }

    WebView {
        id: view

        anchors.fill: parent
        canShowSelectionMarkers: true
        url: root.url

        onUrlChanged: {
            if (fetcher.setConnectUrl(url)) {
                accept();
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
}
