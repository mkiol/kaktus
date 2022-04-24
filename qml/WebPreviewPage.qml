/*
  Copyright (C) 2016 Michal Kosciesza <michal@mkiol.net>

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

// Some ideas heavily inspired and partially borrowed from
// harbour-webpirate project (https://github.com/Dax89/harbour-webpirate)

import QtQuick 2.1
import Sailfish.Silica 1.0
import QtWebKit 3.0

Page {
    id: root

    property bool showBar: false

    property string title
    property string entryId
    property string offlineUrl
    property string onlineUrl
    property bool stared
    property bool liked
    property bool broadcast
    property bool read
    property int index
    property int feedindex
    property bool cached

    property variant _settings: settings
    property int markAsReadTime: 4000
    property int toolbarHideTime: 4000
    property bool readerMode: false
    property bool nightMode: false
    property bool readerModePossible: false
    property bool nightModePossible: true
    property bool autoReaderMode: settings.readerMode
    property bool autoRead: true
    readonly property color bgColor: Theme.colorScheme ? Qt.lighter(Theme.highlightBackgroundColor, 1.9) :
                                                         Qt.darker(Theme.highlightBackgroundColor, 4.0)

    function share() {
        pageStack.push(Qt.resolvedUrl("ShareLinkPage.qml"),{"link": root.onlineUrl, "linkTitle": root.title});
    }

    function openUrlEntryInBrowser(url) {
        Qt.openUrlExternally(url)
    }

    function onlineDownload(url, id) {
        dm.onlineDownload(id, url)
        proggressPanel.text = qsTr("Loading page content...")
        proggressPanel.open = true
    }

    function init() {
        navigate(settings.offlineMode ? offlineUrl : onlineUrl)
    }

    function navigate(url) {
        if (settings.offlineMode) {
            // WORKAROUND for https://github.com/mkiol/kaktus/issues/14
            //utils.resetQtWebKit()
            var xhr = new XMLHttpRequest()
            xhr.onreadystatechange = function () {

                    /*console.log("xhr.onreadystatechange")
                    console.log("  xhr.readyState: " + xhr.readyState)
                    console.log("  xhr.status: " + xhr.status)
                    console.log("  xhr.responseType: " + xhr.responseType)
                    console.log("  xhr.responseURL : " + xhr.responseURL )
                    console.log("  xhr.statusText: " + xhr.statusText)*/

                    if(xhr.readyState === XMLHttpRequest.DONE && xhr.status === 200) {
                        view.loadHtml(xhr.responseText)
                    }

                }
            xhr.open("GET", offlineUrl);
            xhr.send()
        } else {
            view.url = url
        }
    }

    function navigateBack() {
        if (view.canGoBack) {
            root.readerModePossible = false
            root.nightModePossible = false
            view.goBack()
        } else {
            pageStack.pop()
        }
    }

    function initTheme() {
        var theme = { "primaryColor": Theme.rgba(Theme.primaryColor, 1.0).toString(),
                      "secondaryColor": Theme.rgba(Theme.secondaryColor, 1.0).toString(),
                      "highlightColor": Theme.rgba(Theme.highlightColor, 1.0).toString(),
                      "bgColor": root.bgColor.toString(),
                      "fontFamily": Theme.fontFamily,
                      "fontFamilyHeading": Theme.fontFamilyHeading,
                      "pageMargin": Theme.horizontalPageMargin/Theme.pixelRatio,
                      "pageMarginBottom": Theme.itemSizeMedium/Theme.pixelRatio,
                      "fontSize": Theme.fontSizeMedium,
                      "fontSizeTitle": Theme.fontSizeLarge,
                      "zoom": settings.zoom}
        postMessage("theme_set", { "theme": theme })
        postMessage("theme_update_scale")
    }

    function updateZoom(delta) {
        var zoom = settings.zoom;
        settings.zoom = ((zoom + delta) <= 0.5) || ((zoom + delta) >= 2.0) ? zoom : zoom + delta
        var theme = { "zoom": settings.zoom }
        postMessage("theme_set", { "theme": theme })
        postMessage("theme_update_scale")
        baner.show("" + Math.floor(settings.zoom * 100) + "%")
    }

    function switchReaderMode() {
        postMessage(root.readerMode ? "readability_disable" : "readability_enable")
    }

    function switchNightMode() {
        postMessage(root.nightMode ? "nightmode_disable" : "nightmode_enable")
    }

    function messageReceivedHandler(message) {
        //console.log("view.url: " + view.url)
        if (message.type === "inited") {
            // NightMode
            root.nightModePossible = true
            if ((settings.nightMode || root.nightMode) && !settings.offlineMode) {
                postMessage("nightmode_enable")
            } else {
                postMessage("nightmode_disable")
            }
            // Theme
            initTheme()
        } else if (message.type === "readability_result") {
            root.readerModePossible = message.data.possible
            root.readerMode = message.data.enabled

            // Auto switch to reader mode
            if (!root.readerMode && root.readerModePossible &&
                    (root.autoReaderMode || settings.offlineMode)) {
                switchReaderMode()
                root.autoReaderMode = false
            } else if (settings.offlineMode) {
                postMessage("theme_apply")
            }
        } else if (message.type === "readability_status") {
            console.log("readability_status: " + message.data.enabled)
        } else if (message.type === "readability_enabled") {
            root.readerMode = true
            view.scrollToTop()
        } else if (message.type === "readability_disabled") {
            root.readerMode = false
            view.scrollToTop()
        } else if (message.type === "nightmode_enabled") {
            root.nightMode = true
        } else if (message.type === "nightmode_disabled") {
            root.nightMode = false
        }
    }

    function postMessage(message, data) {
        view.experimental.postMessage(JSON.stringify({ "type": message, "data": data }));
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

    Component.onCompleted: init()

    Connections {
        target: fetcher
        onBusyChanged: {
            if(fetcher.busy) {
                pageStack.pop();
            }
        }
    }

    SilicaWebView {
        id: view

        anchors { top: parent.top; left: parent.left; right: parent.right}
        height: parent.height

        experimental.preferences.javascriptEnabled: true
        experimental.preferences.navigatorQtObjectEnabled: true
        experimental.preferredMinimumContentsWidth: 980
        experimental.overview: false
        experimental.enableResizeContent: true
        experimental.userAgent: _settings.getDmUserAgent()
        //experimental.transparentBackground: true

        experimental.userScripts: [
            Qt.resolvedUrl("js/Kaktus.js"),
            Qt.resolvedUrl("js/Console.js"),
            Qt.resolvedUrl("js/MessageListener.js"),
            Qt.resolvedUrl("js/NightMode.js"),
            Qt.resolvedUrl("js/Readability.js"),
            Qt.resolvedUrl("js/Theme.js"),
            Qt.resolvedUrl("js/ReaderMode.js"),
            Qt.resolvedUrl("js/init.js")]

        experimental.onMessageReceived: {
            console.log("onMessageReceived data:", message.data)
            root.messageReceivedHandler(JSON.parse(message.data))
        }

        onLoadingChanged: {
            switch (loadRequest.status) {
            case WebView.LoadStartedStatus:
                proggressPanel.text = qsTr("Loading page content...");
                proggressPanel.open = true;
                break;
            case WebView.LoadSucceededStatus:
                proggressPanel.open = false;

                // Start timer to mark as read
                if (!root.read && root.autoRead)
                    timer.start();

                // Readability.js
                postMessage("readability_apply_fixups")
                postMessage("readability_check", { "title": view.canGoBack ? "" : root.title });

                break;
            case WebView.LoadFailedStatus:
                proggressPanel.open = false;

                if (_settings.offlineMode) {
                    notification.show(qsTr("Failed to load page from local cache"));
                } else {
                    notification.show(qsTr("Failed to load page content"));
                }
                break;
            default:
                proggressPanel.open = false;
            }
        }

        onNavigationRequested: {
            /*console.log("onNavigationRequested: ")
            console.log(" url:",request.url)
            console.log(" navigation type:", request.navigationType)
            console.log(" navigation LinkClickedNavigation:", request.navigationType === WebView.LinkClickedNavigation)
            console.log(" navigation FormSubmittedNavigation:", request.navigationType === WebView.FormSubmittedNavigation)
            console.log(" navigation BackForwardNavigation:", request.navigationType === WebView.BackForwardNavigation)
            console.log(" navigation ReloadNavigation:", request.navigationType === WebView.ReloadNavigation)
            console.log(" navigation FormResubmittedNavigation:", request.navigationType === WebView.FormResubmittedNavigation)
            console.log(" navigation OtherNavigation:", request.navigationType === WebView.OtherNavigation)
            console.log(" action:", request.action);*/

            if (!Qt.application.active) {
                request.action = WebView.IgnoreRequest
                return
            }

            // Offline
            if (settings.offlineMode) {
                if (request.navigationType === WebView.LinkClickedNavigation) {
                    request.action = WebView.IgnoreRequest
                } else {
                    request.action = WebView.AcceptRequest
                }
                return
            }

            // Online
            if (request.navigationType === WebView.LinkClickedNavigation) {
                if (_settings.webviewNavigation === 0) {
                    request.action = WebView.IgnoreRequest;
                    return;
                }

                if (_settings.webviewNavigation === 1) {
                    request.action = WebView.IgnoreRequest;
                    root.openUrlEntryInBrowser(request.url);
                    return;
                }

                if (_settings.webviewNavigation === 2) {
                    request.action = WebView.AcceptRequest
                    return;
                }
            }

            request.action = WebView.AcceptRequest
        }
    }

    TempBaner {
        id: baner
        anchors.centerIn: root
    }

    IconBar {
        id: controlbar
        flickable: view
        color: root.bgColor
        showable: !hideToolbarTimer.running

        IconBarItem {
            text: qsTr("Back")
            icon: "image://theme/icon-m-back"
            onClicked: root.navigateBack()
        }

        IconBarItem {
            text: qsTr("Toggle Read")
            icon: root.read ? "image://icons/icon-m-read-selected" : "image://icons/icon-m-read"
            onClicked: {
                if (root.read) {
                    root.read=false;
                    entryModel.setData(root.index, "read", 0, "");
                } else {
                    root.read=true;
                    entryModel.setData(root.index, "read", 1, "");
                }
            }
        }

        IconBarItem {
            text: app.isNetvibes ? qsTr("Toggle Save") : qsTr("Toggle Star")
            icon: root.stared ? "image://theme/icon-m-favorite-selected" : "image://theme/icon-m-favorite"
            onClicked: {
                if (root.stared) {
                    root.stared=false;
                    entryModel.setData(root.index, "readlater", 0, "");
                } else {
                    root.stared=true;
                    entryModel.setData(root.index, "readlater", 1, "");
                }
            }
        }

        IconBarItem {
            text: qsTr("Toggle Reader View")
            icon: root.readerMode ? "image://icons/icon-m-reader-selected" : "image://icons/icon-m-reader"
            enabled: root.readerModePossible && !settings.offlineMode
            visible: !settings.offlineMode
            onClicked: {
                root.switchReaderMode()
            }
        }

        IconBarItem {
            text: qsTr("Toggle Night View")
            icon: root.nightMode ? "image://icons/icon-m-night-selected" : "image://icons/icon-m-night"
            enabled: !root.readerMode && !settings.offlineMode
            visible: !settings.offlineMode
            onClicked: {
                root.switchNightMode()
            }
        }

        IconBarItem {
            text: qsTr("Browser")
            icon: "image://icons/icon-m-browser"
            onClicked: {
                var url = view.url.toString().lastIndexOf("about") === 0 ||
                          view.url.length === 0 ? root.onlineUrl : view.url
                console.log("Opening: " + url)
                Qt.openUrlExternally(url)
            }
        }

        IconMenuItem_ {
            text: qsTr("Add to Pocket")
            visible: settings.pocketEnabled
            enabled: settings.pocketEnabled && dm.online
            icon.source: "image://icons/icon-m-pocket?" + Theme.primaryColor
            busy: pocket.busy
            onClicked: {
                pocket.add(root.onlineUrl, root.title)
            }
        }

        // not available in harbour package
        IconMenuItem_ {
            text: qsTr("Share link")
            icon.source: "image://theme/icon-m-share"
            onClicked: root.share()
            visible: !settings.isHarbour()
        }

        IconBarItem {
            text: qsTr("Toggle Like")
            icon: root.liked ? "image://icons/icon-m-like-selected" : "image://icons/icon-m-like"
            enabled: settings.showBroadcast && app.isOldReader
            onClicked: {
                entryModel.setData(root.index, "liked", !root.liked, "");
                root.liked = !root.liked
            }
        }

        IconBarItem {
            text: qsTr("Toggle Share")
            icon: root.broadcast ? "image://icons/icon-m-share-selected" : "image://icons/icon-m-share"
            enabled: settings.showBroadcast && app.isOldReader && !root.friendStream
            onClicked: {
                if (root.broadcast) {
                    entryModel.setData(root.index, "broadcast", false, "");
                } else {
                    pageStack.push(Qt.resolvedUrl("ShareDialog.qml"),{"index": root.index});
                }
                root.broadcast = !root.broadcast
            }
        }

        IconBarItem {
            text: qsTr("Copy URL")
            icon: "image://theme/icon-m-clipboard"
            onClicked: {
                notification.show(qsTr("URL was copied to the clipboard"));
                Clipboard.text = root.onlineUrl;
            }
        }

        IconBarItem {
            text: qsTr("Decrease font")
            icon: "image://icons/icon-m-fontdown"
            onClicked: {
                root.updateZoom(-0.1)
            }
        }

        IconBarItem {
            text: qsTr("Increase font")
            icon: "image://icons/icon-m-fontup"
            onClicked: {
                root.updateZoom(0.1)
            }
        }

        IconBarItem {
            text: qsTr("Hide toolbar")
            icon: "image://theme/icon-m-dismiss"
            onClicked: {
                hideToolbarTimer.start()
                controlbar.hide()
            }
        }
    }

    ProgressPanel {
        id: proggressPanel
        anchors.left: parent.left
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
                entryModel.setData(root.index, "read", 1, "");
            }
        }
    }

    Timer {
        id: hideToolbarTimer
        interval: root.toolbarHideTime
    }
}
