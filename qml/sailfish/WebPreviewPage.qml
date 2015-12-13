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
    property bool read
    property int index
    property int feedindex
    property bool cached
    property int markAsReadTime: 4000

    property bool updateViewPortDone: false
    signal updateViewPort

    property int imgWidth: {
        switch (settings.fontSize) {
        case 1:
            return view.width/(1.5);
        case 2:
            return view.width/(2.0);
        }
        return view.width;
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
            root.updateViewPort();
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

    Connections {
        target: fetcher
        onBusyChanged: pageStack.pop()
    }

    /*Connections {
        target: dm
        onBusyChanged: pageStack.pop()
    }*/

    // Workaround for 'High Power Consumption' webkit bug
    Connections {
        target: Qt.application
        onActiveChanged: {
            if(!Qt.application.active && settings.powerSaveMode) {
                pageStack.pop();
            }
        }
    }

    onUpdateViewPort: {
        var viewport = 1;
        if (settings.fontSize==1)
            viewport = 1.5;
        if (settings.fontSize==2)
            viewport = 2.0;
        view.experimental.evaluateJavaScript(
        "(function(){
           var viewport = document.querySelector('meta[name=\"viewport\"]');
           if (viewport) {
             viewport.content = 'initial-scale="+viewport+"';
             return 1;
           }
           document.getElementsByTagName('head')[0].appendChild('<meta name=\"viewport\" content=\"initial-scale="+viewport+"\">');
           return 0;
         })()",
         function(result) {
             //console.log("viewport present:",result);
         });
    }

    SilicaWebView {
        id: view

        //anchors { left: parent.left; right: parent.right }
        //height: controlbar.open ? parent.height - controlbar.height : parent.height
        //clip: true
        anchors { top: parent.top; left: parent.left; right: parent.right; bottom: parent.bottom }

        experimental.userAgent: settings.getDmUserAgent()

        onLoadProgressChanged: {
            // Changing viewport on 50% load proggress in WebView to increase font size
            if (!root.updateViewPortDone &&
                    loadProgress>50 && settings.fontSize>0) {
                root.updateViewPort();
                root.updateViewPortDone = true;
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
                root.updateViewPortDone = false;

                break;
            case WebView.LoadSucceededStatus:
                proggressPanel.open = false;

                // Changing viewport in WebView to increase font size
                root.updateViewPort();

                // Start timer to mark as read
                if (!root.read)
                    timer.start();

                break;
            case WebView.LoadFailedStatus:
                proggressPanel.open = false;

                //console.log("LoadFailedStatus");

                if (settings.offlineMode) {
                    notification.show(qsTr("Failed to load item from local cache :-("));
                } else {
                    if (settings.readerMode) {
                        notification.show(qsTr("Failed to switch to Reader mode :-("));
                        settings.readerMode = false;
                    } else {
                        notification.show(qsTr("Failed to load page content :-("));
                    }
                }
                break;
            default:
                proggressPanel.open = false;
            }
        }

        /*onNavigationRequested: {
            // In Off-Line mode navigation is disabled
            if (settings.offlineMode) {
                if (request.url != offlineUrl) {
                    request.action = WebView.IgnoreRequest;
                }
            }
        }*/

        onNavigationRequested: {
            if (!Qt.application.active) {
                request.action = WebView.IgnoreRequest;
                return;
            }

            if (settings.offlineMode) {
                if (request.navigationType != WebView.LinkClickedNavigation) {
                    return;
                }
                request.action = WebView.IgnoreRequest;
                return;
            }

            if (request.navigationType == WebView.LinkClickedNavigation) {
                onlineUrl = request.url;
                if (settings.readerMode) {
                    //console.log("Reader mode: navigation request url=",request.url);
                    onlineDownload(request.url);
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
        canClipboard: true
        canReader: !settings.offlineMode
        canOpenBrowser: true
        stared: root.stared
        transparent: false

        onBackClicked: pageStack.pop()

        onStarClicked: {
            if (stared) {
                stared=false;
                entryModel.setData(root.index, "readlater", 0, "");
            } else {
                stared=true;
                entryModel.setData(root.index, "readlater", 1, "");
            }
        }

        onBrowserClicked: {
            notification.show(qsTr("Launching an external browser..."));
            Qt.openUrlExternally(onlineUrl);
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
            //pageStack.push(Qt.resolvedUrl("ShareDialog.qml"));
        }
    }

    ProgressPanel {
        id: proggressPanel
        transparent: false
        anchors.left: parent.left
        height: isPortrait ? app.panelHeightPortrait : app.panelHeightLandscape
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
