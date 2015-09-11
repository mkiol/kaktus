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
import bb.device 1.2
import net.mkiol.kaktus 1.0

Sheet {
    id: root
    
    property bool clickable: false
    property int progress: 0
    
    onOpened: {
        progress = 0;
        clickable = false;
        timer.start();
    }
    
    onProgressChanged: {
        if (progress > 9) {
            root.close();
        }
    }
    
    attachedObjects: [
        QTimer {
            id: timer
            interval: 2000
            onTimeout: {
                root.clickable = true;
            }
        }
    ]

    Page {
        Container {
            background: Color.Black
            preferredWidth: app.width
            preferredHeight: app.height
            leftPadding: utils.du(5)
            rightPadding: utils.du(5)

            layout: DockLayout {
            }
            
            Container {
                verticalAlignment: VerticalAlignment.Top
                horizontalAlignment: HorizontalAlignment.Center
                preferredHeight: utils.du(10)
                layout: DockLayout {}
                
                Container {
                    verticalAlignment: VerticalAlignment.Bottom
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    
                    Dot {
                        active: root.progress>=0
                    }
                    Dot {
                        active: root.progress>=1
                    }
                    Dot {
                        active: root.progress>=2
                    }
                    Dot {
                        active: root.progress>=3
                    }
                    Dot {
                        active: root.progress>=4
                    }
                    Dot {
                        active: root.progress>=5
                    }
                    Dot {
                        active: root.progress>=6
                    }
                    Dot {
                        active: root.progress>=7
                    }
                    Dot {
                        active: root.progress>=8
                    }
                    Dot {
                        active: root.progress>=9
                    }
                }
            }
            
            Container {
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Center
                
                Label {
                    text: root.progress == 0 ? qsTr("Hi,\nThis guide will explain you how to use basic features of Kaktus.") :
                    root.progress == 1 ? qsTr("Kaktus is working in sync mode.\n\nAll titles and descriptions of new articles are fetched once during synchronization. Content of the articles are fetched only when caching option is turn on.") :
                          root.progress == 2 ? app.isFeedly ? qsTr("There are 4 view modes.\n\nYou can switch between these modes by selecting appropriate tab.") : qsTr("There are 5 view modes.\n\nYou can switch between these modes by selecting appropriate tabs.") :
                          root.progress == 3 ? app.isNetvibes ? qsTr("Mode #1\n\nLists all your Netvibes tabs. Feeds are grouped by the tabs they belong to and articles are grouped in the feeds.") : qsTr("Mode #1\n\nLists all your folders. Feeds are grouped by the folders they belong to and articles are grouped in the feeds.") :
                          root.progress == 4 ? app.isNetvibes ? qsTr("Mode #2\n\nLists all your Netvibes tabs. Articles are grouped by the tabs they belong to.") : qsTr("Mode #2\n\nLists all your folders. Articles are grouped by the folders they belong to.") :
                          root.progress == 5 ? qsTr("Mode #3\n\nLists all articles from all your feeds in one list. Items are ordered by publication date.") :
                          root.progress == 6 ? app.isNetvibes || app.isFeedly ? qsTr("Mode #4\n\nLists all articles you have saved.") : qsTr("Mode #4\n\nLists all articles you have starred.") :
                          root.progress == 7 ? app.isNetvibes ? qsTr("Mode #5 \"Slow\"\n\nList articles from less frequently updated feeds. A feed is considered \"slow\" when it publishes less than 5 articles in a month.") : qsTr("Mode #5\n\nLists all articles you have liked.") :
                          root.progress == 8 ? qsTr("Top bar contains network indicator.\n\nThis indicator enables you to switch between the online and offline mode. In the offline mode, Kaktus will only use local cache to get web pages and images, so network connection won't be needed.") :
                          root.progress == 9 ? qsTr("That's all!\n\nIf you want to see this guide one more time, click Show User Guide on the settings page.") :
                          ""
                    textStyle.textAlign: TextAlign.Center
                    textStyle.base: SystemDefaults.TextStyles.TitleText
                    textStyle.color: Color.White
                    multiline: true
                }
                
                Container {
                    preferredHeight: utils.du(4)
                    preferredWidth: utils.du(4)
                }

                Container {
                    horizontalAlignment: HorizontalAlignment.Center
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }

                    ImageView {
                        visible: root.progress == 1
                        imageSource: "asset:///sync.png"
                    }
                    ImageView {
                        visible: root.progress == 2 || root.progress == 3
                        imageSource: "asset:///vm0.png"
                    }
                    ImageView {
                        visible: root.progress == 2 || root.progress == 4
                        imageSource: "asset:///vm1.png"
                    }
                    ImageView {
                        visible: root.progress == 2 || root.progress == 5
                        imageSource: "asset:///vm3.png"
                    }
                    ImageView {
                        visible: root.progress == 2 || root.progress == 6
                        imageSource: "asset:///vm4.png"
                    }
                    ImageView {
                        visible: app.isNetvibes ? root.progress == 2 || root.progress == 7 : false
                        imageSource: "asset:///vm5.png"
                    }
                    ImageView {
                        visible: app.isOldReader ? root.progress == 2 || root.progress == 7 : false
                        imageSource: "asset:///vm6.png"
                    }
                    ImageView {
                        visible: root.progress == 8
                        imageSource: "asset:///network-online.png"
                    }
                }
            }

            Container {
                visible: root.clickable && root.progress == 0
                verticalAlignment: VerticalAlignment.Bottom
                horizontalAlignment: HorizontalAlignment.Center
                preferredHeight: utils.du(10)
                Label {
                    text: qsTr("Tap anywhere to continue")
                    textStyle.textAlign: TextAlign.Center
                    textStyle.base: SystemDefaults.TextStyles.BodyText
                    textStyle.color: Color.Gray
                }
            }

            onTouch: {
                if (root.clickable && event.touchType == TouchType.Up) {
                    
                    if (root.progress==6 && app.isFeedly) {
                        root.progress++;
                    }
                    
                    if (root.progress==9) {
                        // Help finished
                        settings.helpDone = true;
                        console.log("Help finished")
                    }
                    
                    progress++;
                }
            }
        }
    }
}