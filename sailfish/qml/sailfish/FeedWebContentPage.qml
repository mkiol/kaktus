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
        bar.hide();
        controlbar.show();
        var fontSize = getFontSize();
        var style = "h1,h2,h3,div,p,pre,code{word-wrap:break-word}body{margin:"+Theme.horizontalPageMargin+";margin-bottom:"+Theme.itemSizeExtraLarge+
                ";background-color:"+Theme.highlightDimmerColor+";"+"color:"+Theme.primaryColor+";"+"font-size:"+fontSize+
                ";font-family:"+Theme.fontFamily+"}"+"a{color:"+Theme.highlightColor+"}"+"img{height:auto;max-width:100%;width:auto;}";
        view.loadHtml(utils.formatHtml(content, settings.offlineMode, style));
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

    function getFontSize() {
        return Theme.fontSizeSmall * (settings.fontSize / 10) * 0.7;
    }

    function updateFontSize() {
        view.experimental.evaluateJavaScript(
        "(function(){document.body.style.fontSize="+getFontSize()+";})()",
         function(result) {
             //console.log("result:",result);
         });
    }

    function check() {
        // Not allowed while Syncing
        if (dm.busy || fetcher.busy || dm.removerBusy) {
            notification.show(qsTr("Please wait until current task is complete."));
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
                notification.show(qsTr("Network connection is unavailable.\nSwitching to Offline mode."));
                settings.offlineMode = true;
            } else {
                // Entry not cached
                notification.show(qsTr("Network connection is unavailable."));
                return false;
            }
        }

        return true;
    }

    function openEntryInBrowser() {
        entryModel.setData(index, "read", 1, "");
        notification.show(qsTr("Launching an external browser..."));
        Qt.openUrlExternally(settings.offlineMode ? offlineUrl : onlineUrl);
    }

    function openUrlEntryInBrowser(url) {
        notification.show(qsTr("Launching an external browser..."));
        Qt.openUrlExternally(url);
    }

    function openEntryInViewer() {

        // (!dm.online && settings.offlineMode) -> WORKAROUND for https://github.com/mkiol/kaktus/issues/14
        if (!dm.online && settings.offlineMode) {
            openEntryInBrowser();
            return;
        }

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

        onNavigationRequested: {
            if (!Qt.application.active) {
                request.action = WebView.IgnoreRequest;
                return;
            }

            // Offline
            if (settings.offlineMode) {
                if (request.navigationType === WebView.LinkClickedNavigation) {
                    request.action = WebView.IgnoreRequest;
                } else {
                    request.action = WebView.AcceptRequest
                }
                return;
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
        }

        header: PageHeader {
            title: root.title
        }
    }

    VerticalScrollDecorator {
        flickable: view
    }

    IconBar {
        id: controlbar
        flickable: view
        transparent: false

        IconBarItem {
            text: qsTr("Back")
            icon: "image://theme/icon-m-back"
            onClicked: pageStack.pop()
        }

        IconBarItem {
            text: qsTr("Toggle Read")
            icon: root.read ? "image://icons/read-selected" : "image://icons/read-notselected"
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
            icon: root.stared ? "image://icons/star-selected" : "image://icons/star-notselected"
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
            icon: "image://icons/webview"
            onClicked: {
                if (!root.check())
                    return;
                root.openEntryInViewer();
            }
        }

        IconBarItem {
            text: qsTr("Browser")
            icon: "image://icons/browser"
            onClicked: {
                root.openEntryInBrowser();
            }
        }

        IconBarItem {
            text: qsTr("Toggle Like")
            icon: root.liked ? "image://icons/like-selected" : "image://icons/like-notselected"
            enabled: settings.showBroadcast && app.isOldReader
            onClicked: {
                entryModel.setData(root.index, "liked", !root.liked, "");
                root.liked = !root.liked
            }
        }

        IconBarItem {
            text: qsTr("Toggle Share")
            icon: root.broadcast ? "image://icons/share-selected" : "image://icons/share-notselected"
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
            text: qsTr("Increase font")
            icon: "image://icons/fontup"
            onClicked: {
                settings.fontSize++;
            }
        }

        IconBarItem {
            text: qsTr("Decrease font")
            icon: "image://icons/fontdown"
            onClicked: {
                settings.fontSize--;
            }
        }

        IconBarItem {
            text: qsTr("Copy URL")
            icon: "image://theme/icon-m-clipboard"
            onClicked: {
                notification.show(qsTr("URL copied to clipboard"));
                Clipboard.text = root.onlineUrl;
            }
        }
    }

    /*ControlBarWebPreview {
        id: controlbar
        flick: view
        canBack: false
        canMarkRead: true
        canStar: true
        canClipboard: true
        canReader: false
        canOpenWebview: true
        canOpenBrowser: !settings.openInBrowser
        stared: root.stared
        read: root.read
        transparent: false

        onBackClicked: pageStack.pop()

        onMarkReadClicked: {
            if (read) {
                read=false;
                entryModel.setData(root.index, "readr", 0, "");
            } else {
                read=true;
                entryModel.setData(root.index, "read", 1, "");
            }
        }

        onStarClicked: {
            if (stared) {
                stared=false;
                entryModel.setData(root.index, "readlater", 0, "");
            } else {
                stared=true;
                entryModel.setData(root.index, "readlater", 1, "");
            }
        }

        onWebviewClicked: {
            if (!check()) {
                return;
            }
            openEntryInViewer();
        }

        onBrowserClicked: {
            openEntryInBrowser();
        }

        onFontDownClicked: {
            if (settings.fontSize>0)
                settings.fontSize -= 1;
        }

        onFontUpClicked: {
            if (settings.fontSize<2)
                settings.fontSize += 1;
        }

        onClipboardClicked: {
            notification.show(qsTr("URL copied to clipboard"));
            Clipboard.text = root.onlineUrl;
        }
    }*/
}
