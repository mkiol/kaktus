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

Page {

    property bool menuEnabled: false
    
    function disconnectSignals() {
    }
    
    titleBar: TitleBar {
        title: qsTr("Add account")
    }
    
    Container {
        layout: DockLayout {}
        
        ListView {
            id: listView
            
            dataModel: ArrayDataModel {
                id: theDataModel
            }
            
            onCreationCompleted: {
                var nv = {name:"Netvibes", icon:"asset:///nv.png", type:1};
                var or = {name:"Old Reader", icon:"asset:///oldreader.png", type:2};
                var fe = {name:"Feedly", icon:"asset:///feedly.png", type:3};
                theDataModel.append(nv);
                theDataModel.append(or);
                theDataModel.append(fe);
            }
            
            verticalAlignment: VerticalAlignment.Top
            
            layout: StackListLayout {
                headerMode: ListHeaderMode.None
            }
            
            listItemComponents: [
                ListItemComponent {
                    type: ""
                    StandardListItem {
                        title: ListItemData.name
                        imageSource: ListItemData.icon
                    }
                }
            ]
            
            onTriggered: {
                var chosenItem = dataModel.data(indexPath);
                if (chosenItem.type == 1) {
                    // Netvibes
                    nav.reconnectFetcher(1);
                    var obj = nvSignInDialog.createObject(); obj.code = 400;
                    var index = nav.indexOf(nav.top);
                    nav.insert(index, obj); nav.navigateTo(nav.at(index)); 
                }
                if (chosenItem.type == 2) {
                    // Old Reader
                    nav.reconnectFetcher(2);
                    var obj = oldReaderSignInDialog.createObject(); obj.code = 400;
                    var index = nav.indexOf(nav.top);
                    nav.insert(index, obj); nav.navigateTo(nav.at(index)); 
                }
                if (chosenItem.type == 3) {
                    // Feedly
                    nav.reconnectFetcher(3);
                    utils.resetQtWebKit();
                    fetcher.getConnectUrl(20);
                }
            }
            
            accessibility.name: "Accounts list"
        }
    }

}
