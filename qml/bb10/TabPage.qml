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
    
    modelType: 0

    titleBar: TitleBar {
        title: qsTr("Tabs")
    }

    attachedObjects: [
        AbstractItemModel {
            id: bbTabModel
            sourceModel: tabModel
        }
    ]

    onCreationCompleted: {
        Qt.tabModel = tabModel;
        nav.topChanged.connect(update);
    }
    
    function disconnectSignals() {
        nav.topChanged.disconnect(update);
    }

    function update() {
        if (nav.top == root) {
            Qt.page = root;
            model.updateFlags();
        }
    }

    Container {
        layout: DockLayout {}
        
        LastSyncIndicator {
            visible: bbTabModel.count!=0
        }

        ListView {
            id: listView

            dataModel: bbTabModel
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
                        imageSource: Qt.cache.getUrlbyUrl(ListItemData.iconUrl)
                        unreadCount: ListItemData.unread
                        fresh: ListItemData.fresh>0
                        
                        property bool showMenuOnPressAndHold: (ListItemData.unread + ListItemData.read) > 0
                        contextMenuHandler: ContextMenuHandler {
                            onPopulating: {
                                if (! showMenuOnPressAndHold)
                                    event.abort();
                            }
                        }
                        
                        // Dynamic creation of new items if last item is compleated
                        /*ListItem.onInitializedChanged: {
                            var index = item.ListItem.indexInSection;
                            //console.log("onInitializedChanged, index:", index, "tabModel.count():", Qt.tabModel.count());
                            if (index == Qt.tabModel.count() - 1) {
                                Qt.tabModel.createItems(index + 1, index + Qt.settings.offsetLimit);
                            }
                        }*/

                        contextActions: [
                            ActionSet {
                                ActionItem {
                                    title: qsTr("Mark as read")
                                    imageSource: "asset:///read.png"
                                    enabled: ListItemData.unread != 0
                                    onTriggered: {
                                        Qt.tabModel.markAsRead(item.ListItem.indexInSection);
                                        Qt.page.updateMarkAllActions();
                                    }
                                }
                                ActionItem {
                                    title: qsTr("Mark as unread")
                                    imageSource: "asset:///unread.png"
                                    enabled: ListItemData.read != 0
                                    onTriggered: {
                                        Qt.tabModel.markAsUnread(item.ListItem.indexInSection);
                                        Qt.page.updateMarkAllActions();
                                    }
                                }
                            }
                        ]
                    }
                }
            ]

            onTriggered: {
                var chosenItem = dataModel.data(indexPath);
                if (settings.viewMode == 0) {
                    utils.setFeedModel(chosenItem.uid);
                    var obj = feedPage.createObject();
                    obj.title = chosenItem.title;
                    nav.push(obj);
                }
                if (settings.viewMode == 1) {
                    utils.setEntryModel(chosenItem.uid);
                    var obj = entryPage.createObject();
                    obj.title = chosenItem.title;
                    nav.push(obj);
                }
            }

            accessibility.name: "Tab list"
        }
        
        ViewPlaceholder {
            text: fetcher.busy ? qsTr("Wait until Sync finish.") : qsTr("No tabs")
            visible: bbTabModel.count==0
        }

        ProgressBar {
        }
    }
}
