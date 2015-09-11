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
import bb.system 1.0
import com.kdab.components 1.0

KaktusPage {

    id: root

    property string title
    
    modelType: 2

    titleBar: TitleBar {
        kind: TitleBarKind.FreeForm
        kindProperties: FreeFormTitleBarKindProperties {
            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                leftPadding: utils.du(2)
                Label {
                    text: {
                        switch (settings.viewMode) {
                            case 3:
                                return qsTr("All feeds");
                            case 4:
                                return settings.signinType<10 ? qsTr("Saved") : qsTr("Starred");
                            case 5:
                                return qsTr("Slow");
                            case 6:
                                return qsTr("Liked");
                            case 7:
                                return qsTr("Shared");
                            default:
                                return root.title;
                        }
                    }
                    textStyle.base: SystemDefaults.TextStyles.TitleText
                    textStyle.fontWeight: FontWeight.W500
                    verticalAlignment: VerticalAlignment.Center
                }
            }

            expandableArea {
                content: SegmentedControl {
                    preferredWidth: app.width
                    Option {
                        text: qsTr("All")
                        value: false
                        selected: !settings.showOnlyUnread
                    }
                    Option {
                        text: qsTr("Only unread")
                        value: true
                        selected: settings.showOnlyUnread
                    }
                    onSelectedValueChanged: {
                        settings.showOnlyUnread = selectedValue;
                    }
                }
                //expanded: settings.viewMode == 4 || settings.viewMode == 6 ? false : true
                expanded: false
                indicatorVisibility: settings.viewMode == 4 || settings.viewMode == 6 ? TitleBarExpandableAreaIndicatorVisibility.Hidden : TitleBarExpandableAreaIndicatorVisibility.Default
                toggleArea: TitleBarExpandableAreaToggleArea.EntireTitleBar
            }
        }
    }
    
    attachedObjects: [
        AbstractItemModel {
            id: bbEntryModel
            sourceModel: entryModel
        }
    ]

    onCreationCompleted: {
        entryModel.ready.connect(resetModel);
        
        //Qt.root = root;
        Qt.entryModel = entryModel;
        Qt.settings = settings;
        Qt.dm = dm;
        Qt.starWasPresed = false;
        Qt.barWasPresed = false;
        Qt.isLight = utils.isLight();
        
        /*if (!settings.getHint1Done()) {
            notification.show(qsTr("One-tap to open article, double-tap to mark as read"));
            settings.setHint1Done(true);
        }*/
    }
    
    onNeedToResetModel: {
        entryModel.init();
    }
    
    function resetModel() {
        bbEntryModel.resetSourceModel();
    }
    
    function disconnectSignals() {
        //entryModel.ready.disconnect(resetModel);
        //settings.showOnlyUnreadChanged.disconnect(refreshActions);
    }

    property variant chosenItem
    property int chosenIndex
    
    function clickHandler() {
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
        //obj.offlineUrl = cache.getUrlbyId(encodeURIComponent(chosenItem.uid));
        obj.title = chosenItem.title;
        obj.stared = chosenItem.readlater==1;
        obj.read = chosenItem.read;
        obj.cached = chosenItem.cached;
        obj.index = chosenIndex;
        obj.broadcast = chosenItem.broadcast;
        obj.liked = chosenItem.liked;
        obj.annotations = chosenItem.annotations;
        //console.log("obj.onlineUrl",obj.onlineUrl);
        //console.log("obj.offlineUrl",obj.offlineUrl);
        nav.push(obj);
        obj.start();
    }
    
    Container {
        layout: DockLayout {}
        
        //preferredHeight: Qt.display.pixelSize.height/2
        //maxHeight: Qt.display.pixelSize.height/2
        //minHeight: Qt.display.pixelSize.height/2
        
        LastSyncIndicator {
            visible: bbEntryModel.count!=0
        }
        
        ListView {
            id: listView
            
            signal itemExpanding(int index)
            
            verticalAlignment: VerticalAlignment.Bottom
            dataModel: bbEntryModel
            visible: bbEntryModel.count!=0
            
            scrollRole: ScrollRole.Main

            layout: StackListLayout {
                headerMode: ListHeaderMode.Standard
            }
            
            function itemType(data, indexPath) {
                return (data.uid == "daterow" ? 'header' : data.uid == 'last' ? 'last' : 'item');
            }

            listItemComponents: [
                ListItemComponent {
                    type: "header"              
                    KaktusEntryHeader {
                        title: ListItemData.title
                    }
                },
                
                ListItemComponent {
                    type: "last"
                                  
                    Container {
                        preferredWidth: Qt.app.width
                        bottomPadding: Qt.utils.du(15)
                    }
                },

                ListItemComponent {
                    type: "item"
                    
                    KaktusEntryItem {
                        id: item
                        
                        property bool showMarkedAsRead: Qt.settings.viewMode!=4 && 
                                                        Qt.settings.viewMode!=6 && 
                                                        Qt.settings.viewMode!=7
                        title: ListItemData.title
                        defaultFeedIcon: ListItemData.feedIcon === "http://s.theoldreader.com/icons/user_icon.png"
                        feedTitle: ListItemData.feedTitle
                        feedIconSource: defaultFeedIcon ? colorSize > 1.5 ? "asset:///contact-text.png" : "asset:///contact.png" : Qt.cache.getUrlbyUrl(ListItemData.feedIcon)

                        imageSource: {
                            if (Qt.settings.showTabIcons && ListItemData.image != "")
                                return Qt.settings.offlineMode ? Qt.cache.getUrlbyUrl(ListItemData.image) : Qt.dm.online ? ListItemData.image : Qt.cache.getUrlbyUrl(ListItemData.image);
                            else
                                return "";
                        }
                        annotations: ListItemData.annotations
                        content: ListItemData.content
                        read: ListItemData.read > 0
                        stared: ListItemData.readlater > 0
                        author: ListItemData.author
                        date: ListItemData.date
                        fresh: ListItemData.fresh
                        broadcast: ListItemData.broadcast
                        liked: ListItemData.liked
                        last: ListItemData.uid == "last"

                        onCreationCompleted: {
                            //console.log()
                            ListItem.view.itemExpanding.connect(doColapse)
                        }

                        function doColapse(index) {
                            if (index == item.ListItem.indexInSection)
                                return;
                            expanded = false;
                        }

                        onStarPressedChanged: {
                            if (! Qt.starPressed) {
                                Qt.starWasPressed = true;
                            }
                        }
                        onBarPressedChanged: {
                            if (! Qt.barPressed) {
                                Qt.barWasPressed = true;
                            }
                        }

                        // Dynamic creation of new items if last item is compleated
                        ListItem.onInitializedChanged: {
                            setIconBgColor();
                            var index = item.ListItem.indexInSection;
                            // Last item is dummy, so checking count-2
                            if (index == Qt.entryModel.count() - 2) {
                                //console.log(">> index:",index,"uid:",ListItemData.uid,"title:",ListItemData.title);
                                Qt.entryModel.createItems(index + 2, Qt.settings.offsetLimit);
                            }
                        }
                        
                        attachedObjects: [
                            ComponentDefinition {
                                id: sharePage
                                source: "SharePage.qml"
                            }
                        ]

                        contextActions: [
                            ActionSet {
                                id: actionSet
                                ActionItem {
                                    id: markReadAction
                                    title: ListItemData.read == 0 ? qsTr("Mark as read") : qsTr("Mark as unread")
                                    enabled: ListItemData.read < 2
                                    imageSource: ListItemData.read == 0 ? "asset:///read.png" : "asset:///unread.png"
                                    onTriggered: {
                                        if (ListItemData.read == 0) {
                                            Qt.entryModel.setData(item.ListItem.indexInSection, "read", 1, "");
                                            //Qt.nav.top.refreshActions();
                                            return;
                                        }
                                        if (ListItemData.read == 1) {
                                            Qt.entryModel.setData(item.ListItem.indexInSection, "read", 0, "");
                                            //Qt.nav.top.refreshActions();
                                            return;
                                        }
                                    }
                                    
                                    onCreationCompleted: {
                                        if (!enabled)
                                            actionSet.remove(markReadAction);   
                                    }
                                }
                                ActionItem {
                                    id: markAboveAction
                                    title: qsTr("Mark above as read")
                                    enabled: item.showMarkedAsRead && item.ListItem.indexInSection > 1
                                    imageSource: "asset:///readabove.png"
                                    onTriggered: {
                                        Qt.entryModel.setAboveAsRead(item.ListItem.indexInSection);
                                    }
                                    
                                    onCreationCompleted: {
                                        if (!enabled)
                                            actionSet.remove(markAboveAction);   
                                    }
                                }
                                ActionItem {
                                    title: ListItemData.readlater == 0 ? Qt.settings.signinType<10 ? qsTr("Save") : qsTr("Star") : Qt.settings.signinType<10 ? qsTr("Unsave") : qsTr("Unstar")
                                    enabled: ListItemData.readlater < 2
                                    imageSource: ListItemData.readlater == 0 ? "asset:///save.png" : "asset:///unsave.png"
                                    onTriggered: {
                                        if (ListItemData.readlater == 0) {
                                            Qt.entryModel.setData(item.ListItem.indexInSection, "readlater", 1, "");
                                            return;
                                        }
                                        if (ListItemData.readlater == 1) {
                                            Qt.entryModel.setData(item.ListItem.indexInSection, "readlater", 0, "");
                                            return;
                                        }
                                    }
                                }
                                
                                ActionItem {
                                    id: likeAction
                                    
                                    property bool enabled2: Qt.settings.signinType >= 10 && 
                                                            Qt.settings.signinType < 20 && 
                                                            Qt.settings.showBroadcast
                                    
                                    title: ListItemData.liked ? qsTr("Unlike") : qsTr("Like")
                                    enabled: enabled2 && !Qt.isLight
                                    imageSource: ListItemData.liked == 0 ? "asset:///like.png" : "asset:///unlike.png"
                                    onTriggered: {
                                        if (!ListItemData.liked) {
                                            Qt.entryModel.setData(item.ListItem.indexInSection, "liked", true, "");
                                            return;
                                        }
                                        if (ListItemData.liked) {
                                            Qt.entryModel.setData(item.ListItem.indexInSection, "liked", false, "");
                                            return;
                                        }
                                    }
                                    
                                    onCreationCompleted: {
                                        if (!enabled2)
                                            actionSet.remove(likeAction);   
                                    }
                                }
                                
                                ActionItem {
                                    id: shareAction
                                    
                                    property bool enabled2: Qt.settings.signinType >= 10 && 
                                                            Qt.settings.signinType < 20 && 
                                                            Qt.settings.showBroadcast && 
                                                            ListItemData.feedId.substring(0,4) !== "user"
                                    
                                    title: ListItemData.broadcast ? Qt.isLight ? qsTr("Unshare (only in pro edition)") : qsTr("Unshare") : Qt.isLight ? qsTr("Share (only in pro edition)") : qsTr("Share with followers")
                                    enabled: enabled2 && !Qt.isLight
                                    imageSource: ListItemData.broadcast ? "asset:///unsharefollowers.png" : "asset:///sharefollowers.png"
                                    onTriggered: {
                                        if (ListItemData.broadcast) {
                                            Qt.entryModel.setData(item.ListItem.indexInSection, "broadcast", false, "");
                                        } else {
                                            var obj = sharePage.createObject(); obj.index = item.ListItem.indexInSection;
                                            Qt.nav.push(obj);
                                        }
                                    }
                                    
                                    onCreationCompleted: {
                                        if (!enabled2)
                                            actionSet.remove(shareAction);   
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
 
                if (chosenItem.uid == "daterow" || chosenItem.uid == "last")
                    return;

                // Expander clicked
                if (Qt.barWasPressed) {
                    Qt.barWasPressed = false;
                    // Expander was clicked, so emitting colapse signal
                    itemExpanding(chosenIndex);
                    return;
                }

                // Star cklicked
                if (Qt.starWasPressed) {
                    Qt.starWasPressed = false;
                    if (chosenItem.readlater < 2) {
                        if (chosenItem.readlater == 0) {
                            entryModel.setData(indexPath, "readlater", 1, "");
                            return;
                        }
                        if (chosenItem.readlater == 1) {
                            entryModel.setData(indexPath, "readlater", 0, "");
                            return;
                        }
                    }
                    return;
                }
                
                // Item cklicked
                clickHandler();
            }

            accessibility.name: "Entry list"
        }
        
        ViewPlaceholder {
            text: fetcher.busy ? qsTr("Wait until Sync finish.") :
                  settings.viewMode==4 ? app.isNetvibes || app.isFeedly ? qsTr("No saved items") : qsTr("No starred items")  :
                  settings.viewMode==6 ? qsTr("No liked items") : settings.showOnlyUnread ? qsTr("No unread items") : qsTr("No items")
            
            visible: bbEntryModel.count==0
        }
        
        //ProgressBar {}
    }
}
