/*
 * Copyright (C) 2015 Michal Kosciesza <michal@mkiol.net>
 * 
 * This file is part of Kaktus.
 * 
 * Kaktus is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Kaktus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Kaktus.  If not, see <http://www.gnu.org/licenses/>.
 */

import bb.cascades 1.2
import net.mkiol.kaktus 1.0

Page {
    id: root
    
    property string title
    property string entryId
    property string offlineUrl
    property string onlineUrl
    property bool stared
    property bool read
    property int index
    property int feedindex
    property bool broadcast
    property string annotations
    property bool liked
    property bool cached
    property int markAsReadTime: 4000
    
    property variant g_settings: settings
    property double viewPort: settings.fontSize==1 ? 1.0 : settings.fontSize==2 ? 2.0 : 0.5
    property int imgWidth: app.width / viewPort
    
    signal updateViewPort
    
    actionBarAutoHideBehavior: ActionBarAutoHideBehavior.HideOnScroll
    actionBarVisibility: ChromeVisibility.Overlay

    onCreationCompleted: {
        Qt.isLight = utils.isLight();
        settings.fontSizeChanged.connect(fontSizeChanged);
        settings.readerModeChanged.connect(readerModeChanged);
        dm.onlineDownloadReady.connect(onlineDownloadReady);
        dm.onlineDownloadFailed.connect(onlineDownloadFailed);
    }
    
    function start() {
        if (settings.offlineMode) {
            navigate(root.offlineUrl);
        } else {
            if (settings.readerMode) {
                //console.log("settings.readerMode=1 root.onlineUrl",onlineUrl);
                onlineDownload(root.onlineUrl, root.entryId);
            } else {
                //console.log("settings.readerMode=0 root.onlineUrl",onlineUrl);
                view.url = root.onlineUrl;
            }
        }
    }
    
    function disconnectSignals() {
        settings.fontSizeChanged.disconnect(fontSizeChanged);
        settings.readerModeChanged.disconnect(readerModeChanged);
        dm.onlineDownloadReady.disconnect(onlineDownloadReady);
        dm.onlineDownloadFailed.disconnect(onlineDownloadFailed);
    }
    
    function fontSizeChanged() {
        updateViewPort();
    }
    
    function readerModeChanged() {
        if (settings.readerMode) {
            onlineDownload(root.onlineUrl, "");
        } else {
            view.url = root.onlineUrl;
        }
    }
    
    function onlineDownloadReady(id, url) {
        //console.log("onOnlineDownloadReady url=",url);
        if (id=="") {
            var newUrl = cache.getUrlbyUrl(url);
            console.log("newurl=",newUrl);
            navigate(newUrl);
            root.offlineUrl = newUrl;
            return;
        }
        navigate(root.offlineUrl);
        entryModel.setData(index,"cached",1);
    }
    
    function onlineDownloadFailed() {
        notification.show(qsTr("Failed to switch to Reader mode :-("));
        progressIndicator.hide();
        //settings.readerMode = false;
    }
    
    function onlineDownload(url, id) {
        //console.log("onlineDownload url=",url);
        dm.onlineDownload(id, url);
        progressIndicator.show(qsTr("Loading page content..."));
    }
    
    function navigate(url) {
        // ----- workaround ---------
        var hcolor = "0092cc";
        var shcolor = "0092cc";
        var padding = utils.du(1);
        view.url = url+"?fontsize=18px&width="+root.imgWidth+"&highlightColor="+
            hcolor+"&secondaryHighlightColor="+shcolor+"&margin="+padding;
    }
    
    onUpdateViewPort: {
        var viewport = 1.0;
        if (g_settings.fontSize==0)
            viewport = 0.5;
        if (g_settings.fontSize==1)
            viewport = 1.0;
        if (g_settings.fontSize==2)
            viewport = 1.5;        
        view.evaluateJavaScript("(function(){var viewport = document.querySelector('meta[name=\"viewport\"]');if (viewport) {viewport.content = 'initial-scale="+viewport+"';return 1;} document.getElementsByTagName('head')[0].appendChild('<meta name=\"viewport\" content=\"initial-scale="+viewport+"\">');return 0;})()");
    }

    attachedObjects: [
        QTimer {
            id: timer
            singleShot: true
            interval: markAsReadTime
            onTimeout: {
                if (! root.read) {
                    root.read = true;
                    entryModel.setData(root.index, "read", 1, "");
                }
            }
        },
        ComponentDefinition {
            id: sharePage
            source: "SharePage.qml"
        }
    ]

    actions: [
        ActionItem {
            title: stared ? settings.signinType < 10 ? qsTr("Unsave") : qsTr("Unstar") : settings.signinType < 10 ? qsTr("Save") : qsTr("Star") 
            ActionBar.placement: ActionBarPlacement.OnBar
            imageSource: stared ? "asset:///unsave.png" : "asset:///save.png"
            onTriggered: {
                entryModel.setData(index, "readlater", stared ? 0 : 1, "");
                stared = !stared;
            }
        },
        ActionItem {
            title: qsTr("Copy")
            imageSource: "asset:///copy_link.png"
            ActionBar.placement: ActionBarPlacement.Default
            onTriggered: {
                notification.show(qsTr("URL copied to clipboard"));
                utils.copyToClipboard(root.onlineUrl);
            }
        },
        InvokeActionItem {
            title: qsTr("Share")
            ActionBar.placement: ActionBarPlacement.Default
            query {
                mimeType: "text/plain"
                invokeActionId: "bb.action.SHARE"
            }
            onTriggered: {
                data = root.onlineUrl;
            }
        },
        ActionItem {
            id: likeAction
            property bool enabled2: app.isOldReader && settings.showBroadcast
            title: root.liked ? 
            Qt.isLight ? qsTr("Unlike (only in pro edition)") : qsTr("Unlike") : 
            Qt.isLight ? qsTr("Like (only in pro edition)") : qsTr("Like")
            enabled: enabled2 && !Qt.isLight
            imageSource: root.liked ? "asset:///unlike.png" : "asset:///like.png"
            onTriggered: {
                entryModel.setData(index, "liked", !root.liked, "");
                root.liked = !root.liked;
            }
            
            onCreationCompleted: {
                if (!enabled2)
                    root.removeAction(likeAction);
            }
        },
        ActionItem {
            id: shareAction
            property bool enabled2: app.isOldReader && settings.showBroadcast
            title: root.broadcast ? 
                Qt.isLight ? qsTr("Unshare (only in pro edition)") : qsTr("Unshare") : 
                Qt.isLight ? qsTr("Share (only in pro edition)") : qsTr("Share with followers")
            enabled: enabled2 && !Qt.isLight
            imageSource: root.broadcast ? "asset:///unsharefollowers.png" : "asset:///sharefollowers.png"
            onTriggered: {
                if (root.broadcast) {
                    entryModel.setData(index, "broadcast", false, "");
                } else {
                    var obj = sharePage.createObject(); obj.index = index;
                    nav.push(obj);
                }
                root.broadcast = !root.broadcast;
            }
            
            onCreationCompleted: {
                if (!enabled2)
                    root.removeAction(shareAction);
            }
        },
        ActionItem {
            title: qsTr("Browser")
            imageSource: "asset:///browser.png"
            ActionBar.placement: ActionBarPlacement.Default
            onTriggered: {
                notification.show(qsTr("Launching an external browser..."));
                utils.launchBrowser(onlineUrl);
            }
        },
        /*ActionNetworkMode {
            enabled: !utils.isLight()
            onTriggered: {
                if (utils.isLight()) {
                    notification.show(qsTr("Offline mode is available only in pro edition of Kaktus."));
                    return;
                }
                if (settings.offlineMode) {
                    if (dm.online)
                        settings.offlineMode = false;
                    else
                        notification.show(qsTr("Cannot switch to Online mode.\nNetwork connection is unavailable."));
                } else {
                    if (root.cached)
                        settings.offlineMode = true;
                    else
                        notification.show(qsTr("Offline version not available."));
                }
            }
        }*/
        ActionItem {
            title: settings.readerMode ? qsTr("Read mode: on") : qsTr("Read mode: off")
            enabled: !settings.offlineMode
            imageSource: settings.readerMode ? "asset:///reader.png" : "asset:///reader-disabled.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                settings.readerMode = !settings.readerMode;
            }
        }
    ]

    Container {
        layout: DockLayout {}

        ScrollView {
            id: scrollView
            
            scrollViewProperties {
                scrollMode: ScrollMode.Both
                pinchToZoomEnabled: true
            }
            
            scrollRole: ScrollRole.Main
            
            WebView {
                id: view

                settings.viewport: {
                    "width": "device-width",
                    "initial-scale": 1.0
                }

                //settings.userAgent: g_settings.getDmUserAgent()
                
                onLoadProgressChanged: {
                    progressIndicator.progress = loadProgress / 100.0;
                }
                
                onMinContentScaleChanged: {
                    scrollView.scrollViewProperties.minContentScale = minContentScale;
                    scrollView.zoomToPoint(0, 0, minContentScale, ScrollAnimation.None)
                }
                
                onMaxContentScaleChanged: {
                    scrollView.scrollViewProperties.maxContentScale = maxContentScale;
                }
                
                onLoadingChanged: {
                    if (loadRequest.status == WebLoadStatus.Started) {
                        progressIndicator.show(qsTr("Loading page content..."));
                    } else if (loadRequest.status == WebLoadStatus.Succeeded) {
                        
                        // Changing viewport in WebView to increase font size
                        root.updateViewPort();
                        
                        progressIndicator.hide();
                        timer.start();
                    } else if (loadRequest.status == WebLoadStatus.Failed) {
                        progressIndicator.hide();
                        
                        if (g_settings.offlineMode) {
                            notification.show(qsTr("Failed to load item from local cache :-("));
                        } else {
                            if (g_settings.readerMode) {
                                notification.show(qsTr("Failed to switch to Reader mode :-("));
                                g_settings.readerMode = false;
                            } else {
                                notification.show(qsTr("Failed to load page content :-("));
                            }
                        }
                        
                    }
                }
                
                onNavigationRequested: {
                    if (g_settings.offlineMode) {
                        if (request.navigationType != WebNavigationType.LinkClicked) {
                            return;
                        }
                        request.action = WebNavigationRequestAction.Ignore;
                        return;
                    }
                    
                    if (request.navigationType == WebNavigationType.LinkClicked) {
                        root.onlineUrl = request.url;
                        if (g_settings.readerMode) {
                            //console.log("Reader mode: navigation request url=",request.url);
                            onlineDownload(request.url);
                            request.action = WebNavigationRequestAction.Ignore;
                        }
                    }
                }
            }
        }

        Container {
            verticalAlignment: VerticalAlignment.Top
            horizontalAlignment: HorizontalAlignment.Left

            ProgressPanel {
                id: progressIndicator
                open: false
                onCancelClicked: {
                    view.stop();
                }
            }
        }
    }
}
