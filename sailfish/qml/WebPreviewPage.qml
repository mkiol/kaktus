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
    property bool updateStyleDone: false

    property int imgWidth: {
        switch (settings.fontSize) {
        case 1:
            return view.width/(1.5);
        case 2:
            return view.width/(2.0);
        }
        return view.width;
    }

    function openUrlEntryInBrowser(url) {
        notification.show(qsTr("Launching an external browser..."));
        Qt.openUrlExternally(url);
    }

    function onlineDownload(url, id) {
        //console.log("onlineDownload url=",url);
        dm.onlineDownload(id, url);
        proggressPanel.text = qsTr("Loading page content...");
        proggressPanel.open = true;
    }

    function navigate(url) {
        var hcolor = Theme.highlightColor.toString().substr(1, 6);
        var shcolor = Theme.secondaryHighlightColor.toString().substr(1, 6);
        view.url = url+"?fontsize=18px&width="+imgWidth+"&highlightColor="+hcolor+"&secondaryHighlightColor="+shcolor+"&margin="+Theme.paddingMedium;
    }

    function updateStyle() {
        var viewport = settings.fontSize / 10;
        viewport = viewport < 1 ? viewport.toPrecision(1) : viewport.toPrecision(2);

        view.experimental.evaluateJavaScript(
        "(function(){
           // viewport
           var viewport = document.querySelector('meta[name=\"viewport\"]');
           if (viewport) {
             viewport.content = 'initial-scale="+viewport+"';
           } else {
             document.getElementsByTagName('head')[0].appendChild('<meta name=\"viewport\" content=\"initial-scale="+viewport+"\">');
           }

           // bottom margin
           document.body.style.marginBottom=\""+Theme.itemSizeExtraLarge+"px\";
           //document.body.style.maxWidth=\"100%\";
           //document.body.style.width=\"100%\";
           return 0;
         })()",
         function(result) {
             //console.log("result:",result);
         });
    }

    ActiveDetector {}

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

    Component.onCompleted: {
        if (settings.offlineMode) {
            navigate(offlineUrl);
        } else {
            if (settings.readerMode) {
                onlineDownload(root.onlineUrl, root.entryId);
            } else {
                view.url = onlineUrl;
            }
        }
    }

    Connections {
        target: settings
        onReaderModeChanged: {
            if (settings.readerMode) {
                //onlineDownload(root.onlineUrl, root.entryId);
                onlineDownload(root.onlineUrl, "");
            } else {
                view.url = onlineUrl;
            }
        }

        onFontSizeChanged: {
            // Changing viewport in WebView
            updateStyle();
        }
    }

    Connections {
        target: dm
        onOnlineDownloadReady: {
            //console.log("onOnlineDownloadReady url=",url);
            if (id=="") {
                var newUrl = cache.getUrlbyUrl(url);
                //console.log("newurl=",newUrl);
                navigate(newUrl);
                offlineUrl = newUrl;
                return;
            }
            navigate(offlineUrl);
            entryModel.setData(index,"cached",1, "");
        }
        onOnlineDownloadFailed: {
            notification.show(qsTr("Failed to switch to Reader mode :-("));
            proggressPanel.open = false;
            //settings.readerMode = false;
        }
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
        //height: parent.height - (controlbar.shown ? controlbar.height : 0)
        height: parent.height

        //overridePageStackNavigation: true

        experimental.userAgent: _settings.getDmUserAgent()
        experimental.transparentBackground: _settings.offlineMode || _settings.readerMode
        experimental.overview: false
        experimental.enableResizeContent: true

        onLoadProgressChanged: {
            // Changing viewport on 50% load proggress in WebView to increase font size
            if (loadProgress>50) {
                root.updateStyle();
                root.updateStyleDone = true;
            }
        }

        onLoadingChanged: {

            /*console.log(">>> onLoadingChanged");
            console.log("loadRequest.url=",loadRequest.url);
            console.log("loadRequest.status=",loadRequest.status);
            console.log("loadRequest.errorString=",loadRequest.errorString);
            console.log("loadRequest.errorCode=",loadRequest.errorCode);
            console.log("loadRequest.errorDomain=",loadRequest.errorDomain);*/

            switch (loadRequest.status) {
            case WebView.LoadStartedStatus:
                proggressPanel.text = qsTr("Loading page content...");
                proggressPanel.open = true;

                // Reseting viewport flag
                root.updateStyleDone = false;

                break;
            case WebView.LoadSucceededStatus:
                proggressPanel.open = false;

                // Changing viewport in WebView to increase font size
                root.updateStyle();

                // Start timer to mark as read
                if (!root.read)
                    timer.start();

                break;
            case WebView.LoadFailedStatus:
                proggressPanel.open = false;

                //console.log("LoadFailedStatus");

                if (_settings.offlineMode) {
                    notification.show(qsTr("Failed to load item from local cache :-("));
                } else {
                    if (_settings.readerMode) {
                        notification.show(qsTr("Failed to switch to Reader mode :-("));
                        _settings.readerMode = false;
                    } else {
                        notification.show(qsTr("Failed to load page content :-("));
                    }
                }
                break;
            default:
                proggressPanel.open = false;
            }
        }

        onNavigationRequested: {

            //console.log("onNavigationRequested, URL:",request.url,"navigationType:",request.navigationType);

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
                    onlineUrl = request.url;
                    if (_settings.readerMode) {
                        //console.log("Reader mode: navigation request url=",request.url);
                        onlineDownload(request.url);
                        request.action = WebView.IgnoreRequest;
                        return;
                    }
                    request.action = WebView.AcceptRequest
                    return;
                }
            }

            request.action = WebView.AcceptRequest
        }
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
            text: qsTr("Toggle Read mode")
            icon: settings.readerMode ? "image://icons/reader-selected" : "image://icons/reader-notselected"
            enabled: !settings.offlineMode
            onClicked: {
                settings.readerMode = !settings.readerMode;
            }
        }

        IconBarItem {
            text: qsTr("Browser")
            icon: "image://icons/browser"
            onClicked: {
                notification.show(qsTr("Launching an external browser..."));
                Qt.openUrlExternally(onlineUrl);
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
                settings.fontSize++
            }
        }

        IconBarItem {
            text: qsTr("Decrease font")
            icon: "image://icons/fontdown"
            onClicked: {
                settings.fontSize--
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

    ProgressPanel {
        id: proggressPanel
        transparent: false
        anchors.left: parent.left
        //height: isPortrait ? app.panelHeightPortrait : app.panelHeightLandscape
        cancelable: true
        onCloseClicked: view.stop()

        y: controlbar.open ? root.height-height-controlbar.height : root.height-height
        Behavior on y { NumberAnimation { duration: 200;easing.type: Easing.OutQuad } }
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
}
