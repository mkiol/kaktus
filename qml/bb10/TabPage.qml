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
import com.kdab.components 1.0

KaktusPage {

    id: root
    
    modelType: 0

    titleBar: TitleBar {
        //scrollBehavior: TitleBarScrollBehavior.Sticky
        title: settings.signinType<10 ? qsTr("Tabs") : qsTr("Folders")
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
        //settings.showOnlyUnreadChanged.disconnect(refreshActions);
    }

    function update() {
        if (nav.top == root) {
            Qt.page = root;
            model.updateFlags();
            //refreshActions();
        }
    }

    Container {
        layout: DockLayout {}
        
        LastSyncIndicator {
            visible: bbTabModel.count!=0
        }

        ListView {
            id: listView
            
            scrollRole: ScrollRole.Main

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
                        text: ListItemData.uid === "subscriptions" ? qsTr("Subscriptions") : 
                              ListItemData.uid === "friends" ? qsTr("Following") : 
                              ListItemData.title
                        imageSource: ListItemData.uid === "friends" ? Application.themeSupport.theme.colorTheme.style === VisualStyle.Bright ? "asset:///contact-text.png" : "asset:///contact.png" : ListItemData.iconUrl === "" ? "" : Qt.cache.getUrlbyUrl(ListItemData.iconUrl)
                        imageBackgroundVisible: false
                        unreadCount: ListItemData.unread
                        fresh: ListItemData.fresh > 0
                        last: ListItemData.uid == "last"

                        property bool showMenuOnPressAndHold: (ListItemData.unread + ListItemData.read) > 0
                        contextMenuHandler: ContextMenuHandler {
                            onPopulating: {
                                if (! showMenuOnPressAndHold)
                                    event.abort();
                            }
                        }
                        
                        ListItem.onInitializedChanged: {
                            setIconBgColor();
                        }

                        contextActions: [
                            ActionSet {
                                id: actionSet
                                ActionItem {
                                    title: qsTr("Mark as read")
                                    imageSource: "asset:///read.png"
                                    enabled: ListItemData.unread != 0
                                    onTriggered: {
                                        Qt.tabModel.markAsRead(item.ListItem.indexInSection);
                                        Qt.page.updateMarkAllActions();
                                        //Qt.nav.top.refreshActions();
                                    }
                                }
                                ActionItem {
                                    id: unreadAction
                                    title: qsTr("Mark as unread")
                                    imageSource: "asset:///unread.png"
                                    enabled: ListItemData.read != 0
                                    onTriggered: {
                                        Qt.tabModel.markAsUnread(item.ListItem.indexInSection);
                                        Qt.page.updateMarkAllActions();
                                    }
                                    
                                    onCreationCompleted: {
                                        if (Qt.settings.signinType < 10)
                                            actionSet.remove(unreadAction);   
                                    }
                                }
                            }
                        ]
                    }
                }
            ]

            onTriggered: {
                var chosenItem = dataModel.data(indexPath);
                /*if (chosenItem.uid == "friends" && utils.isLight()) {
                    notification.show(qsTr("The feature is available only in the pro edition"));
                    return;
                }*/
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
            text: fetcher.busy ? qsTr("Wait until Sync finish.") : 
                                 settings.signinType<10 ? qsTr("No tabs") : qsTr("No folders")
            visible: bbTabModel.count==0
        }
    }
}
