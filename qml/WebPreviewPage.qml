/* Copyright (C) 2014-2022 Michal Kosciesza <michal@mkiol.net>
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

    readonly property variant _settings: settings
    readonly property int _markAsReadTime: 4000
    property bool _readerMode: false
    property int _nightMode: settings.nightMode ? 1 : 0
    property bool _readerModePossible: false
    property bool _nightModePossible: false
    readonly property bool _autoReaderMode: settings.readerMode
    property bool _zoomPossible: false
    property bool autoRead: true
    readonly property color _bgColor: Theme.colorScheme === Theme.LightOnDark ?
                                          Qt.darker(Theme.highlightBackgroundColor, 5.0) :
                                          Qt.lighter(Theme.highlightBackgroundColor, 1.8)

    function init() {
        navigate(settings.offlineMode ? offlineUrl : onlineUrl)
    }

    function navigate(url) {
        if (settings.offlineMode) {
            view.url = offlineUrl
        } else {
            view.url = url
        }
    }

    function navigateBack() {
        if (view.canGoBack) {
            root._readerModePossible = false
            root._nightModePossible = false
            view.goBack()
        } else {
            pageStack.pop()
        }
    }

    function init_js() {
        var script =
                utils.readAsset("scripts/Readability.js") + "\n" +
                utils.readAsset("scripts/reader_view.js") + "\n" +
                utils.readAsset("scripts/night_mode.js") + "\n" +
                utils.readAsset("scripts/zoom.js") + "\n" +
                "var res = {reader_view: false,night_mode: false,zoom: false}\n" +
                "try {\n" +
                "res.reader_view = _reader_view_init()\n" +
                "res.night_mode = _night_mode_init()\n" +
                "res.zoom = _zoom_init()\n" +
                "} catch {}\n" +
                "return res\n";
        view.runJavaScript(script, function(res) {
            console.log("js init done:", JSON.stringify(res))
            root._readerModePossible = res.reader_view
            root._nightModePossible = res.night_mode
            root._zoomPossible = res.zoom
            if (root._readerModePossible) setReaderMode(root._readerMode)
            if (root._nightModePossible) setNightMode(root._nightMode)
            if (root._zoomPossible) setZoom(settings.zoomFontSize())
            controlbar.show()
        }, errorCallback)
    }

    function errorCallback(error) {
        console.log("error:", error)
    }

    function updateZoom(delta) {
        if (!root._zoomPossible) return
        settings.zoom = settings.zoom + delta
        setZoom(settings.zoomFontSize())
        baner.show(Math.round(settings.zoom * 100).toString() + "%")
    }

    function setZoom(zoom) {
        if (!root._zoomPossible) return
        var script = "return window._zoom_set('" + zoom + "')\n";
        view.runJavaScript(script, function(res) {
            console.log("zoom set done:", zoom, res)
        }, errorCallback)
    }

    function setNightMode(type) {
        if (!root._nightModePossible) return
        var script = "return window._night_mode_set(" + type + ")\n"

        view.runJavaScript(script, function(res) {
            console.log("night switch done:", type, res)
            if (res) root._nightMode = type
        }, errorCallback)
    }

    function switchNightMode() {
        if (root._readerMode) {
            setNightMode(root._nightMode == 0 ? 1 : root._nightMode == 1 ? 0 : 0)
        } else {
            setNightMode(root._nightMode == 0 ? 1 : root._nightMode == 1 ? 2 : 0)
        }
    }

    function setReaderMode(enabled) {
        if (!root._readerModePossible) return
        var script = "return window._reader_view_set(" + (enabled ? "true" : "false") + ")\n"
        view.runJavaScript(script, function(res) {
            console.log("reader mode switch done:", enabled, res)
            if (res) root._readerMode = enabled
        }, errorCallback)
    }

    function switchReaderMode() {
        setReaderMode(!root._readerMode)
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

    onStatusChanged: {
        if (status === PageStatus.Active) {
            if (view.url.toString().length === 0) init()
            controlbar.show()
        } else {
            controlbar.hide()
        }
    }

    WebView {
        id: view

        anchors.fill: parent
        canShowSelectionMarkers: true

        onLoadedChanged: {
            if (loaded) {
                root.init_js()
                if (!root.read && root.autoRead) timer.start();
            }
        }
    }

    TempBaner {
        id: baner
        anchors.centerIn: root
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
            icon: root._readerMode ? "image://icons/icon-m-reader-selected" : "image://icons/icon-m-reader"
            enabled: root._readerModePossible && !settings.offlineMode
            visible: !settings.offlineMode
            onClicked: {
                root.switchReaderMode()
            }
        }

        IconBarItem {
            text: qsTr("Toggle Night View")
            icon: root._readerMode ? root._nightMode === 1 || root._nightMode === 2 ?
                                         "image://icons/icon-m-night2" :
                                         "image://icons/icon-m-night0" :
                  root._nightMode === 1 ? "image://icons/icon-m-night1" :
                  root._nightMode === 2 ? "image://icons/icon-m-night2" :
                                          "image://icons/icon-m-night0"
            enabled: root._nightModePossible
            visible: true
            onClicked: {
                root.switchNightMode()
            }
        }

        IconBarItem {
            text: qsTr("Browser")
            icon: "image://icons/icon-m-browser"
            onClicked: {
                var url = view.url.toString().lastIndexOf("about") === 0 ||
                          view.url.toString().length === 0 ? root.onlineUrl : view.url
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
            enabled: root._zoomPossible
            visible: true
            onClicked: {
                root.updateZoom(-0.1)
            }
        }

        IconBarItem {
            text: qsTr("Increase font")
            icon: "image://icons/icon-m-fontup"
            enabled: root._zoomPossible
            visible: true
            onClicked: {
                root.updateZoom(0.1)
            }
        }

        IconBarItem {
            text: qsTr("Hide toolbar")
            icon: "image://theme/icon-m-dismiss"
            onClicked: {
                controlbar.hide()
            }
        }
    }

    Timer {
        id: timer
        interval: root._markAsReadTime
        onTriggered: {
            if (!root.read) {
                read=true;
                entryModel.setData(root.index, "read", 1, "");
            }
        }
    }
}
