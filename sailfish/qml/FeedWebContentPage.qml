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

import QtQuick 2.0
import Sailfish.Silica 1.0
import QtWebKit 3.0

Page {
    id: root

    property bool showBar: false

    property string title
    property string content
    property string entryId
    property string offlineUrl
    property string onlineUrl
    property bool stared
    property bool liked
    property bool read
    property bool broadcast
    property int index
    property int feedindex
    property bool cached

    property variant _settings: settings
    property bool themeApply: true

    property bool navigateBackPop: true

    function init() {
        view.loadHtml(utils.formatHtml(content, settings.offlineMode, ""))
        navigateBackPop = true
        themeApply = true
    }

    function navigateBack() {
        if (view.canGoBack)
            view.goBack()
        else
            if (navigateBackPop)
                pageStack.pop()
            else
                init()
    }

    allowedOrientations: {
        switch (settings.allowedOrientations) {
        case 1:
            return Orientation.Portrait;
        case 2:
            return Orientation.Landscape;
        }
        return Orientation.Landscape | Orientation.Portrait;
    }

    Component.onCompleted: {
        bar.hide()
        controlbar.show()
        init()
    }

    // Workaround for 'High Power Consumption' webkit bug
    Connections {
        target: Qt.application
        onActiveChanged: {
            if(!Qt.application.active) {
                if (settings.powerSaveMode && root.status === PageStatus.Active) {
                    pageStack.pop()
                    return
                }
                if (root.status !== PageStatus.Active) {
                    pageStack.pop(pageStack.previousPage(root), PageStackAction.Immediate)
                    return
                }
            }
        }
    }

    Connections {
        target: fetcher
        onBusyChanged: {
            if(fetcher.busy) {
                pageStack.pop();
            }
        }
    }

    function share() {
        pageStack.push(Qt.resolvedUrl("ShareLinkPage.qml"),{"link": root.onlineUrl, "linkTitle": root.title});
    }

    function check() {
        // Not allowed while Syncing
        if (dm.busy || fetcher.busy || dm.removerBusy) {
            notification.show(qsTr("Wait until current task is complete."));
            return false;
        }

        // Entry not cached and offline mode enabled
        if (settings.offlineMode && !cached) {
            notification.show(qsTr("Offline version not available."));
            return false;
        }

        // Switch to Offline mode if no network
        if (!settings.offlineMode && !dm.online) {
            if (cached) {
                // Entry cached
                notification.show(qsTr("Enabling offline mode because network is disconnected."));
                settings.offlineMode = true;
            } else {
                // Entry not cached
                notification.show(qsTr("Network is disconnected."));
                return false;
            }
        }

        return true;
    }

    function openEntryInBrowser() {
        entryModel.setData(index, "read", 1, "");
        notification.show(qsTr("Launching an external browser..."));
        //Qt.openUrlExternally(settings.offlineMode ? offlineUrl : onlineUrl);
        Qt.openUrlExternally(onlineUrl)
    }

    function openUrlEntryInBrowser(url) {
        notification.show(qsTr("Launching an external browser..."));
        Qt.openUrlExternally(url);
    }

    function openEntryInViewer() {
        pageStack.replace(Qt.resolvedUrl("WebPreviewPage.qml"),
                          {"entryId": entryId,
                              "onlineUrl": onlineUrl,
                              "offlineUrl": offlineUrl,
                              "title": title,
                              "stared": stared,
                              "liked": liked,
                              "broadcast": broadcast,
                              "index": index,
                              "feedindex": feedindex,
                              "read" : read,
                              "cached" : cached
                          });
    }

    function openUrlInViewer(url) {
        pageStack.replace(Qt.resolvedUrl("WebPreviewPage.qml"),
                          {"entryId": entryId,
                              "onlineUrl": url,
                              "offlineUrl": url,
                              "title": title,
                              "stared": stared,
                              "liked": liked,
                              "broadcast": broadcast,
                              "index": index,
                              "feedindex": feedindex,
                              "read" : read,
                              "cached" : cached
                          });
    }

    function openEntry() {
        if (!check()) {
            return;
        }

        if (settings.clickBehavior === 1) {
            openEntryInBrowser();
            return;
        }

        openEntryInViewer();
    }

    function initTheme() {
        var theme = { "primaryColor": Theme.rgba(Theme.primaryColor, 1.0).toString(),
                      "secondaryColor": Theme.rgba(Theme.secondaryColor, 1.0).toString(),
                      "highlightColor": Theme.rgba(Theme.highlightColor, 1.0).toString(),
                      "highlightColorDark": Qt.darker(Theme.highlightColor).toString(),
                      "secondaryHighlightColor": Theme.rgba(Theme.secondaryHighlightColor, 1.0).toString(),
                      "highlightDimmerColor": Theme.rgba(Theme.highlightDimmerColor, 1.0).toString(),
                      "fontFamily": Theme.fontFamily,
                      "fontFamilyHeading": Theme.fontFamilyHeading,
                      "pageMargin": Theme.horizontalPageMargin/Theme.pixelRatio,
                      "pageMarginBottom": Theme.itemSizeMedium/Theme.pixelRatio,
                      "fontSize": Theme.fontSizeMedium,
                      "fontSizeTitle": Theme.fontSizeLarge,
                      "zoom": settings.zoom,
                      "theme": settings.readerTheme }
        postMessage("theme_set", { "theme": theme })
        postMessage("theme_apply")
    }

    function updateZoom(delta) {
        var zoom = settings.zoom;
        settings.zoom = ((zoom + delta) <= 0.5) || ((zoom + delta) >= 2.0) ? zoom : zoom + delta
        var theme = { "zoom": settings.zoom }
        postMessage("theme_set", { "theme": theme })
        postMessage("theme_apply")
        baner.show("" + Math.floor(settings.zoom * 100) + "%")
    }

    function messageReceivedHandler(message) {
        if (message.type === "inited") {
            if (root.themeApply) {
                initTheme()
                root.themeApply = false
            }
            postMessage("readability_apply_fixups")
        }
    }

    function postMessage(message, data) {
        view.experimental.postMessage(JSON.stringify({ "type": message, "data": data }));
    }

    Connections {
        target: settings
        onFontSizeChanged: updateFontSize()
    }

    Rectangle {
        anchors.fill: parent
        color: Theme.highlightDimmerColor
    }

    SilicaWebView {
        id: view

        anchors.fill: parent
        experimental.transparentBackground: true
        experimental.overview: false
        experimental.enableResizeContent: true
        experimental.preferences.javascriptEnabled: true
        experimental.preferences.navigatorQtObjectEnabled: true

        /*onLoadingChanged: {
            console.log("onLoadingChanged:")
            console.log(" url: ", loadRequest.url)
            console.log(" status: ", loadRequest.status)
            console.log(" error string: ", loadRequest.errorString)
            console.log(" error code:: ", loadRequest.errorCode)

            if (loadRequest.status === WebView.LoadSucceededStatus) {
                console.log(" LoadSucceededStatus")
            }
        }*/

        experimental.userScripts: [
            Qt.resolvedUrl("js/Kaktus.js"),
            Qt.resolvedUrl("js/Console.js"),
            Qt.resolvedUrl("js/MessageListener.js"),
            Qt.resolvedUrl("js/Readability.js"),
            Qt.resolvedUrl("js/Theme.js"),
            Qt.resolvedUrl("js/ReaderMode.js"),
            Qt.resolvedUrl("js/init.js")]

        experimental.onMessageReceived: {
            console.log("onMessageReceived data:", message.data)
            root.messageReceivedHandler(JSON.parse(message.data))
        }

        onNavigationRequested: {
            if (!Qt.application.active) {
                request.action = WebView.IgnoreRequest;
                return
            }

            /*console.log("onNavigationRequested: ")
            console.log(" url:",request.url)
            console.log(" navigation type:", request.navigationType)
            console.log(" navigation LinkClickedNavigation:", request.navigationType === WebView.LinkClickedNavigation)
            console.log(" navigation FormSubmittedNavigation:", request.navigationType === WebView.FormSubmittedNavigation)
            console.log(" navigation BackForwardNavigation:", request.navigationType === WebView.BackForwardNavigation)
            console.log(" navigation ReloadNavigation:", request.navigationType === WebView.ReloadNavigation)
            console.log(" navigation FormResubmittedNavigation:", request.navigationType === WebView.FormResubmittedNavigation)
            console.log(" navigation OtherNavigation:", request.navigationType === WebView.OtherNavigation)
            console.log(" action:", request.action)*/

            if (request.url.toString() === root.onlineUrl ||
                    request.url.toString() === root.offlineUrl) {

                if (_settings.webviewNavigation === 0) {
                    request.action = WebView.IgnoreRequest
                    return
                }

                if (_settings.webviewNavigation === 1 && !_settings.offlineMode) {
                    request.action = WebView.IgnoreRequest
                    root.openUrlEntryInBrowser(request.url)
                    return
                }

                root.openEntryInViewer()
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
                    request.action = WebView.IgnoreRequest
                    return
                }

                if (_settings.webviewNavigation === 1) {
                    request.action = WebView.IgnoreRequest
                    root.openUrlEntryInBrowser(request.url)
                    return
                }

                if (_settings.webviewNavigation === 2) {
                    root.openUrlInViewer(request.url)
                    request.action = WebView.IgnoreRequest
                    return;
                }
            }
        }

        header: PageHeader {
            title: root.title
        }
    }

    VerticalScrollDecorator {
        flickable: view
    }

    TempBaner {
        id: baner
        anchors.centerIn: root
    }

    IconBar {
        id: controlbar
        flickable: view
        theme: "dimmer"

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
            text: app.isNetvibes || app.isFeedly ?
                  qsTr("Toggle Save") : qsTr("Toggle Star")
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
            text: qsTr("Viewer")
            icon: "image://icons/icon-m-webview"
            onClicked: {
                if (!root.check())
                    return;
                root.openEntryInViewer();
            }
        }

        IconBarItem {
            text: qsTr("Browser")
            icon: "image://icons/icon-m-browser"
            onClicked: {
                root.openEntryInBrowser();
            }
        }

        IconMenuItem {
            text: qsTr("Add to Pocket")
            visible: settings.pocketEnabled
            enabled: settings.pocketEnabled && dm.online
            icon.source: "image://icons/icon-m-pocket"
            busy: pocket.busy
            onClicked: {
                pocket.add(root.onlineUrl, root.title)
            }
        }

        IconMenuItem {
            text: qsTr("Share link")
            icon.source: "image://theme/icon-m-share"
            onClicked: root.share()
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
                notification.show(qsTr("URL was copied to the clipboard."));
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
    }
}
