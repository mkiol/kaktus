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
import bb.system 1.0
import com.kdab.components 1.0
import net.mkiol.kaktus 1.0

KaktusPage {

    id: root

    property string title
    
    modelType: 2

    titleBar: TitleBar {
        title: {
            switch (settings.viewMode) {
                case 3:
                    return qsTr("All feeds");
                case 4:
                    return qsTr("Saved");
                case 5:
                    return qsTr("Slow");
                default:
                    return root.title;
            }
        }
    }
    
    attachedObjects: [
        AbstractItemModel {
            id: bbEntryModel
            sourceModel: entryModel
        },
        //Double click Timer
        QTimer {
            id: dclickTimer
            singleShot: true
            interval: 400
            onTimeout: {
                // One click
                oneClick();
            }
        }
    ]

    onCreationCompleted: {
        Qt.entryModel = entryModel;
        Qt.settings = settings;
        Qt.dm = dm;
        Qt.starWasPresed = false;
        Qt.barWasPresed = false;
    }
    
    function disconnectSignals() {
    }

    property variant chosenItem
    property int chosenIndex
    
    function oneClick() {
        // Not allowed while Syncing
        if (dm.busy || fetcher.busy || dm.removerBusy) {
            notification.show(qsTr("Please wait until current task is complete."));
            return;
        }
        
        // Entry not cached and offline mode enabled
        if (settings.offlineMode && !chosenItem.cached) {
            notification.show(qsTr("Offline version not available."));
            return;
        }
        
        // Switch to Offline mode if no network
        if (!settings.offlineMode && !dm.online) {
            if (chosenItem.cached) {
                // Entry cached
                notification.show(qsTr("Network connection is unavailable.\nSwitching to Offline mode."));
                settings.offlineMode = true;
            } else {
                // Entry not cached
                notification.show(qsTr("Network connection is unavailable."));
                return;
            }
        }
        
        var obj = webPreviewPage.createObject();
        obj.onlineUrl = chosenItem.link;
        obj.offlineUrl = cache.getUrlbyId(chosenItem.uid);
        obj.title = chosenItem.title;
        obj.stared = chosenItem.readlater==1;
        obj.read = chosenItem.read;
        obj.cached = chosenItem.cached;
        obj.index = chosenIndex;
        nav.push(obj);
    }

    Container {
        layout: DockLayout {}
        
        LastSyncIndicator {
            visible: bbEntryModel.count!=0
        }
        
        ListView {
            id: listView
            
            signal itemExpanding(int index)
            
            verticalAlignment: VerticalAlignment.Top
            dataModel: bbEntryModel
            visible: bbEntryModel.count!=0

            layout: StackListLayout {
                headerMode: ListHeaderMode.None
            }

            listItemComponents: [
                ListItemComponent {
                    type: ""

                    KaktusEntryItem {
                        id: item

                        title: ListItemData.title
                        feedIconSource: Qt.settings.viewMode == 1 || Qt.settings.viewMode == 3 || Qt.settings.viewMode == 4 || Qt.settings.viewMode == 5 ? Qt.cache.getUrlbyUrl(ListItemData.feedIcon) : ""
                        imageSource: {
                            if (Qt.settings.showTabIcons && ListItemData.image != "")
                                return Qt.settings.offlineMode ? Qt.cache.getUrlbyUrl(ListItemData.image) : Qt.dm.online ? ListItemData.image : Qt.cache.getUrlbyUrl(ListItemData.image);
                            else
                                return "";
                        }
                        content: ListItemData.content
                        read: ListItemData.read > 0
                        stared: ListItemData.readlater > 0
                        author: ListItemData.author
                        date: ListItemData.date
                        fresh: ListItemData.fresh
                        
                        onCreationCompleted: {
                            ListItem.view.itemExpanding.connect(doColapse)
                        }
                        
                        function doColapse(index) {
                            if (index == item.ListItem.indexInSection)
                                return;
                            expanded = false;
                        }

                        onStarPressedChanged: {
                            if (!Qt.starPressed) {
                                Qt.starWasPressed = true;
                            }
                        }
                        onBarPressedChanged: {
                            if (!Qt.barPressed) {
                                Qt.barWasPressed = true;
                            }
                        }
                        
                        // Dynamic creation of new items if last item is compleated
                        ListItem.onInitializedChanged: {
                            var index = item.ListItem.indexInSection;
                            //console.log("onInitializedChanged, index:", index, "entryModel.count():", Qt.entryModel.count());
                            if (index == Qt.entryModel.count() - 1) {
                                Qt.entryModel.createItems(index + 1, index + Qt.settings.offsetLimit);
                            }
                        }
                        
                        contextActions: [
                            ActionSet {
                                ActionItem {
                                    title: ListItemData.read == 0 ? qsTr("Mark as read") : qsTr("Mark as unread")
                                    enabled: ListItemData.read < 2
                                    imageSource: ListItemData.read == 0 ? "asset:///read.png" : "asset:///unread.png"
                                    onTriggered: {
                                        if (ListItemData.read == 0) {
                                            Qt.entryModel.setData(item.ListItem.indexInSection, "read", 1);
                                            return;
                                        }
                                        if (ListItemData.read == 1) {
                                            Qt.entryModel.setData(item.ListItem.indexInSection, "read", 0);
                                            return;
                                        }
                                    }
                                }
                                ActionItem {
                                    title: ListItemData.readlater == 0 ? qsTr("Save") : qsTr("Unsave")
                                    enabled: ListItemData.readlater < 2
                                    imageSource: ListItemData.readlater == 0 ? "asset:///save.png" : "asset:///unsave.png"
                                    onTriggered: {
                                        if (ListItemData.readlater == 0) {
                                            Qt.entryModel.setData(item.ListItem.indexInSection, "readlater", 1);
                                            return;
                                        }
                                        if (ListItemData.readlater == 1) {
                                            Qt.entryModel.setData(item.ListItem.indexInSection, "readlater", 0);
                                            return;
                                        }
                                    }
                                }
                            }
                        ]
                    }
                }
            ]

            onTriggered: {
                chosenItem = dataModel.data(indexPath);
                chosenIndex = indexPath[0];
                
                if (Qt.barWasPressed) {
                    Qt.barWasPressed = false;
                    // Expander was clicked, so emitting colapse signal
                    itemExpanding(chosenIndex);
                    return;
                }
                
                if (Qt.starWasPressed) {
                    Qt.starWasPressed = false;
                    if (chosenItem.readlater < 2) {
                        if (chosenItem.readlater == 0) {
                            entryModel.setData(indexPath, "readlater", 1);
                            return;
                        }
                        if (chosenItem.readlater == 1) {
                            entryModel.setData(indexPath, "readlater", 0);
                            return;
                        }
                    }
                    return;
                }
                
                if (dclickTimer.active) {
                    // Double click
                    dclickTimer.stop();
                    // Marking as read / unread
                    if (chosenItem.read < 2) {
                        if (chosenItem.read == 0) {
                            entryModel.setData(indexPath, "read", 1);
                            return;
                        }
                        if (chosenItem.read == 1) {
                            entryModel.setData(indexPath, "read", 0);
                            return;
                        }
                    }
                } else {
                    dclickTimer.start();
                }
            }

            accessibility.name: "Entry list"
        }
        
        ViewPlaceholder {
            text: fetcher.busy ? qsTr("Wait until Sync finish.") :
            settings.viewMode==4 ? qsTr("No saved items") :
            settings.showOnlyUnread ? qsTr("No unread items") : qsTr("No items")
            visible: bbEntryModel.count==0
        }
        
        ProgressBar {}
    }
}
