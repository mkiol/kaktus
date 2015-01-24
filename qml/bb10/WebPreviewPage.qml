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

import bb.cascades 1.3
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
    property bool cached
    property int markAsReadTime: 4000
    
    property variant g_settings: settings
    
    signal updateViewPort

    onCreationCompleted: {
        //settings.offlineModeChanged.connect(networkModeChanged);
        settings.fontSizeChanged.connect(fontSizeChanged);
        //fetcher.busyChanged.connect(busyChanged);
        //dm.busyChanged.connect(busyChanged);
    }
    
    function disconnectSignals() {
        //settings.offlineModeChanged.disconnect(networkModeChanged);
        settings.fontSizeChanged.disconnect(fontSizeChanged);
        //fetcher.busyChanged.disconnect(busyChanged);
        //dm.busyChanged.disconnect(busyChanged);
    }
    
    function fontSizeChanged() {
        updateViewPort();
    }
    
    /*function networkModeChanged() {
        if (settings.offlineMode) {
            if (!root.cached) {
                notification.show(qsTr("Offline version not available."));
                disconnectSignals();
                nav.remove(root);
                root.destroy();
            }
        }
    }
    function busyChanged() {
        if (fetcher.busy || dm.busy()) {
            disconnectSignals();
            nav.remove(root);
            root.destroy();
        }
    }*/
    
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
                if (!root.read) {
                    root.read=true;
                    entryModel.setData(root.index, "read", 1);
                }
            }
        }
    ]

    actions: [
        ActionItem {
            title: stared ? qsTr("Unsave") : qsTr("Save")
            ActionBar.placement: ActionBarPlacement.OnBar
            //imageSource: stared ? "asset:///unsave.png" : "asset:///save.png"
            imageSource: stared ? "asset:///star-selected.png" : "asset:///star.png"
            onTriggered: {
                entryModel.setData(index, "readlater", stared ? 0 : 1);
                stared = !stared;
            }
        },
        ActionItem {
            title: qsTr("Browser")
            imageSource: "asset:///browser.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                notification.show(qsTr("Launching an external browser..."));
                utils.launchBrowser(onlineUrl);
            }
        },
        ActionNetworkMode {
            onTriggered: {
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
            
            WebView {
                id: view

                property double viewPort: g_settings.fontSize==1 ? 1.0 : g_settings.fontSize==2 ? 2.0 : 0.5
                property int imgWidth: display.pixelSize.width / viewPort

                url: g_settings.offlineMode ? offlineUrl + "?fontsize=18px&width=" + imgWidth + "px" : onlineUrl

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
                        if (g_settings.offlineMode)
                            notification.show(qsTr("Failed to load item from local cache :-("));
                        else
                            notification.show(qsTr("Failed to load page content :-("));
                        break;
                        
                    }
                }

                /*onNavigationRequested: {
                    console.debug("NavigationRequested: " + request.url + " navigationType=" + request.navigationType)
                }
                
                onMessageReceived: {
                    console.debug("message.origin: " + message.origin);
                    console.debug("message.data: " + message.data);
                }*/
            }
        }

        Container {
            verticalAlignment: VerticalAlignment.Bottom
            horizontalAlignment: HorizontalAlignment.Left

            Container {
                background: ui.palette.plainBase
                preferredHeight: ui.du(0.25)
                preferredWidth: display.pixelSize.width
            }

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
