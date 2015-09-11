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
import "const.js" as Theme

Page {
    id: root
    
    property bool menuEnabled: false
    property int code
    
    function disconnectSignals() {
    }
    
    titleBar: TitleBar {
        title: qsTr("Account")
    }
    
    ScrollView {
        
        Container {
            topPadding: utils.du(2)
            bottomPadding: utils.du(2)
            
            Header {
                title: qsTr("Feedly")
            }
            
            Container {
                leftPadding: utils.du(2)
                rightPadding: utils.du(2)
                topPadding: utils.du(2)
                bottomPadding: utils.du(2)
                
                Container {
                    preferredWidth: display.pixelSize.width
                    
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    
                    Container {
                        topPadding: utils.du(1)
                        ImageView {
                            imageSource: "asset:///feedly.png"
                            verticalAlignment: VerticalAlignment.Top
                            preferredWidth: 46
                            minWidth: 46
                        }
                    }
                }
            }
            
            Container {
                minWidth: utils.du(2)
            }
            
                Button {
                    text: qsTr("Sign in")
                    verticalAlignment: VerticalAlignment.Center
                    preferredWidth: display.pixelSize.width
                    onClicked: {
                        utils.resetQtWebKit();
                        fetcher.getConnectUrl(3);
                    }
                }
        }
    }
    
    actions: [
        ActionItem {
            id: action1
            title: qsTr("Sign in")
            imageSource: "asset:///save_folder.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                accept();
            }
        }
    ]
    
    function accept() {
        utils.resetQtWebKit();
        fetcher.getConnectUrl(3);
    }
}
