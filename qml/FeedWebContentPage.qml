/* Copyright (C) 2016-2022 Michal Kosciesza <michal@mkiol.net>
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
    property bool autoRead: true

    property variant _settings: settings
    property bool _zoomPossible: false
    property bool _themePossible: false
    readonly property color _bgColor: Theme.colorScheme === Theme.LightOnDark ?
                                          Qt.darker(Theme.highlightBackgroundColor, 5.0) :
                                          Qt.lighter(Theme.highlightBackgroundColor, 1.8)
    property bool _navigateBackPop: false

    function init() {
        view.loadHtml(utils.formatHtml(content, settings.offlineMode, ""))
        _navigateBackPop = true
        if (!read) {
            read = true
            entryModel.setData(index, "read", 1, "")
        }
    }

    function navigateBack() {
        if (view.canGoBack) {
            view.goBack()
        } else {
            if (root._navigateBackPop) pageStack.pop()
            else init()
        }
    }

    allowedOrientations: {
        switch (settings.allowedOrientations) {
        case 1:
            return Orientation.PortraitMask
        case 2:
            return Orientation.LandscapeMask
        }
        return Orientation.All
    }

    Component.onCompleted: {
        bar.hide()
        controlbar.show()
        root.init()
    }

    Connections {
        target: fetcher
        onBusyChanged: {
            if(fetcher.busy) {
                pageStack.pop()
            }
        }
    }

    function share() {
        pageStack.push(Qt.resolvedUrl("ShareLinkPage.qml"),{"link": root.onlineUrl, "linkTitle": root.title});
    }

    function check() {
        // Not allowed while Syncing
        if (dm.busy || fetcher.busy || dm.removerBusy) {
            notification.show(qsTr("Wait until current task is complete"))
            return false
        }

        // Entry not cached and offline mode enabled
        if (settings.offlineMode && !cached) {
            notification.show(qsTr("Offline version not available"))
            return false
        }

        // Switch to Offline mode if no network
        if (!settings.offlineMode && !dm.online) {
            if (cached) {
                // Entry cached
                notification.show(qsTr("Enabling offline mode because network is disconnected"))
                settings.offlineMode = true
            } else {
                // Entry not cached
                notification.show(qsTr("Network is disconnected"))
                return false
            }
        }

        return true
    }

    function openEntryInBrowser() {
        Qt.openUrlExternally(onlineUrl)
    }

    function openEntryInViewer() {
        app.hideBar()
        pageStack.replace(Qt.resolvedUrl("WebPreviewPage.qml"),
                          {
                              "onlineUrl": onlineUrl,
                              "offlineUrl": offlineUrl,
                              "title": title,
                              "stared": stared,
                              "liked": liked,
                              "broadcast": broadcast,
                              "index": index,
                              "feedindex": feedindex,
                              "read" : read,
                              "cached" : cached,
                              "autoRead" : autoRead
                          });
    }

    function openUrlInViewer(url) {
        app.hideBar()
        pageStack.replace(Qt.resolvedUrl("WebPreviewPage.qml"),
                          {
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
        if (!check()) return
        if (settings.clickBehavior === 1) {
            openEntryInBrowser()
            return
        }
        openEntryInViewer()
    }

    function errorCallback(error) {
        console.log("error:", error)
    }

    function init_js() {
        var s = {
                    primaryColor: Theme.rgba(Theme.primaryColor, 1.0).toString(),
                    secondaryColor: Theme.rgba(Theme.secondaryColor, 1.0).toString(),
                    highlightColor: Theme.rgba(Theme.highlightColor, 1.0).toString(),
                    bgColor: Theme.rgba(root._bgColor, 1.0).toString(),
                    fontFamily: Theme.fontFamily,
                    fontFamilyHeading: Theme.fontFamilyHeading,
                    pageMargin: Theme.horizontalPageMargin/Theme.pixelRatio,
                    pageMarginBottom: Theme.itemSizeMedium/Theme.pixelRatio,
                    fontSize: Theme.fontSizeMedium,
                    fontSizeTitle: Theme.fontSizeLarge
                }
        console.log(JSON.stringify(s))
        var script =
                utils.readAsset("scripts/zoom.js") + "\n" +
                utils.readAsset("scripts/theme.js") + "\n" +
                "var res = {theme: false,zoom: false}\n" +
                "try {\n" +
                "res.theme = _theme_init(" + JSON.stringify(s) + ")\n" +
                "res.zoom = _zoom_init()\n" +
                "} catch {}\n" +
                "return res\n";
        view.runJavaScript(script, function(res) {
            console.log("js init done:", JSON.stringify(res))
            root._zoomPossible = res.zoom
            root._themePossible = res.theme
            if (root._zoomPossible) setZoom(settings.zoomViewport(), true)
            if (root._themePossible) setTheme(true)
            controlbar.show()
        }, errorCallback)
    }

    function updateZoom(delta) {
        if (!root._zoomPossible) return
        settings.zoom = settings.zoom + delta
        setZoom(settings.zoomViewport(), false)
        baner.show(Math.round(settings.zoom * 100).toString() + "%")
    }

    function setZoom(zoom, init) {
        if (!root._zoomPossible) return
        var script;
        if (init) script = "return window._zoom_set('" + zoom + "', true)\n";
        else script = "return window._zoom_set('" + zoom + "', false)\n";
        view.runJavaScript(script, function(res) {
            if (init) setZoom(zoom, false)
            else console.log("zoom set done:", zoom, res)
        }, errorCallback)
    }

    function setTheme(enabled) {
        if (!root._themePossible) return
        var script = "return window._theme_set(" + (enabled ? "true" : "false") + ")\n";
        view.runJavaScript(script, function(res) {
            console.log("theme set done:", enabled, res)
        }, errorCallback)
    }

    Connections {
        target: settings
        onFontSizeChanged: setZoom(settings.zoomFontSize())
    }

    Rectangle {
        anchors.fill: parent
        color: root._bgColor
    }

    PageHeader {
        title: root.title
    }

    WebView {
        id: view

        anchors.fill: parent
        canShowSelectionMarkers: true

        onLoadedChanged: {
            if (loaded) {
                root.init_js()
            }
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
                root.autoRead=false
                if (root.read) {
                    root.read=false
                    entryModel.setData(root.index, "read", 0, "")
                } else {
                    root.read=true
                    entryModel.setData(root.index, "read", 1, "")
                }
            }
        }

        IconBarItem {
            text: app.isNetvibes ?
                  qsTr("Toggle Save") : qsTr("Toggle Star")
            icon: root.stared ? "image://theme/icon-m-favorite-selected" : "image://theme/icon-m-favorite"
            onClicked: {
                if (root.stared) {
                    root.stared=false
                    entryModel.setData(root.index, "readlater", 0, "")
                } else {
                    root.stared=true
                    entryModel.setData(root.index, "readlater", 1, "")
                }
            }
        }

        IconBarItem {
            text: qsTr("Viewer")
            icon: "image://icons/icon-m-webview"
            onClicked: {
                if (!root.check()) return
                root.openEntryInViewer()
            }
        }

        IconBarItem {
            text: qsTr("Browser")
            icon: "image://icons/icon-m-browser"
            onClicked: {
                Qt.openUrlExternally(settings.offlineMode ? root.offlineUrl : root.onlineUrl)
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
                entryModel.setData(root.index, "liked", !root.liked, "")
                root.liked = !root.liked
            }
        }

        IconBarItem {
            text: qsTr("Toggle Share")
            icon: root.broadcast ? "image://icons/icon-m-share-selected" : "image://icons/icon-m-share"
            enabled: settings.showBroadcast && app.isOldReader && !root.friendStream
            onClicked: {
                if (root.broadcast) {
                    entryModel.setData(root.index, "broadcast", false, "")
                } else {
                    pageStack.push(Qt.resolvedUrl("ShareDialog.qml"),{"index": root.index})
                }
                root.broadcast = !root.broadcast
            }
        }

        IconBarItem {
            text: qsTr("Copy URL")
            icon: "image://theme/icon-m-clipboard"
            onClicked: {
                notification.show(qsTr("URL was copied to the clipboard"))
                Clipboard.text = root.onlineUrl
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
    }
}
