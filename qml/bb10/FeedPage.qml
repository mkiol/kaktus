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
import com.kdab.components 1.0

KaktusPage {
    
    id: root
    
    property string title
    
    modelType: 1
    
    titleBar: TitleBar {
        title: settings.viewMode==2 ? qsTr("Feeds") : root.title
    }
    
    attachedObjects: [
        AbstractItemModel {
            id: bbFeedModel
            sourceModel: feedModel
        }
    ]
    
    onCreationCompleted: {
        Qt.feedModel = feedModel;
        nav.topChanged.connect(update);
    }
    
    function disconnectSignals() {
        nav.topChanged.disconnect(update);
    }
    
    function update() {
        if (nav.top == root) {
            feedModel.updateFlags();
        }
    }
    
    Container {
        layout: DockLayout {
        }
        
        LastSyncIndicator {
            visible: bbFeedModel.count!=0
        }
        
        ListView {
            id: listView
            
            dataModel: bbFeedModel
            verticalAlignment: VerticalAlignment.Top
            visible: dataModel.count!=0
            
            layout: StackListLayout {
                headerMode: ListHeaderMode.None
            }
            
            listItemComponents: [
                ListItemComponent {
                    type: ""
                    KaktusListItem {
                        id: item
                        text: ListItemData.title
                        imageSource: Qt.cache.getUrlbyUrl(ListItemData.icon)
                        unreadCount: ListItemData.unread
                        fresh: ListItemData.fresh>0
                        
                        property bool showMenuOnPressAndHold: (ListItemData.unread+ListItemData.read)>0
                        contextMenuHandler: ContextMenuHandler {
                            onPopulating: {
                                if (!showMenuOnPressAndHold)
                                    event.abort();
                            }
                        } 
                        
                        // Dynamic creation of new items if last item is compleated
                        /*ListItem.onInitializedChanged: {
                            var index = item.ListItem.indexInSection;
                            //console.log("onInitializedChanged, index:", index, "feedModel.count():", Qt.feedModel.count());
                            if (index == Qt.feedModel.count() - 1) {
                                Qt.feedModel.createItems(index + 1, index + Qt.settings.offsetLimit);
                            }
                        }*/
                        
                        contextActions: [
                            ActionSet {
                                ActionItem {
                                    title: qsTr("Mark as read")
                                    imageSource: "asset:///read.png"
                                    enabled: ListItemData.unread != 0
                                    onTriggered: {
                                        Qt.feedModel.markAsRead(item.ListItem.indexInSection);
                                    }
                                }
                                ActionItem {
                                    title: qsTr("Mark as unread")
                                    imageSource: "asset:///unread.png"
                                    enabled: ListItemData.read != 0
                                    onTriggered: {
                                        Qt.feedModel.markAsUnread(item.ListItem.indexInSection);
                                    }
                                }
                            
                            }
                        ]
                    }
                }
            ]
            
            onTriggered: {
                var chosenItem = dataModel.data(indexPath);
                utils.setEntryModel(chosenItem.uid);

                var obj = entryPage.createObject();
                obj.title = chosenItem.title;
                nav.push(obj);
            }
            
            accessibility.name: "Feed list"
        }
        
        ViewPlaceholder {
            text: fetcher.busy ? qsTr("Wait until Sync finish.") : qsTr("No feeds")
            visible: bbFeedModel.count==0
        }
        
        ProgressBar {}
    }
}
