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
    
    property string url
    property int type
    property int code
    
    property variant g_settings: settings
    
    function disconnectSignals() {
    }
    
    function accept() {
        var doInit = settings.signinType!=type;
        settings.signinType = type;

        if (dm.busy)
            dm.cancel();
        if (doInit)
            fetcher.init();
        else
            fetcher.update();

        if (nav.count()==3) {
            nav.insert(0, firstPage.createObject());
            nav.navigateTo(nav.at(0)); 
        } else {
            nav.pop();
        }
    }
    
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
                
                url: decodeURIComponent(root.url);
                
                settings.viewport: {
                    "width": "device-width",
                    "initial-scale": 1.0
                }
                
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
                        progressIndicator.hide();
                    } else if (loadRequest.status == WebLoadStatus.Failed) {
                        progressIndicator.hide();
                        //notification.show(qsTr("Failed to load page content :-("));
                        // Do something here.....
                        //notification.show(qsTr("Failed to load page content :-("));
                        //nav.pop();
                    }
                }
                
                onUrlChanged: {
                    //console.log("Url changed:", url);
                    if (fetcher.setConnectUrl(url)) {
                        accept();
                    }
                } 
            }
        }
        
        Container {
            verticalAlignment: VerticalAlignment.Bottom
            horizontalAlignment: HorizontalAlignment.Left
            
            Container {
                background: utils.plainBase()
                preferredHeight: utils.du(0.25)
                preferredWidth: app.width
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
