/*
  Copyright (C) 2014 Michal Kosciesza <michal@mkiol.net>

  This file is part of Kaktus.

  Kaktus is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Kaktus is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Kaktus.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 1.1
import com.nokia.meego 1.0

import "Theme.js" as Theme

Page {
    id: root

    property string title
    property int index

    tools: MainToolbar {
        menu: menuItem
    }

    MainMenu {
        id: menuItem
    }

    orientationLock: {
        switch (settings.allowedOrientations) {
        case 1:
            return PageOrientation.LockPortrait;
        case 2:
            return PageOrientation.LockLandscape;
        }
        return PageOrientation.Automatic;
    }

    ListView {
        id: listView

        model: entryModel

        PullBar {}

        header: PageHeader {
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

        anchors.fill: parent

        clip: true

        delegate: EntryDelegate {

            id: listItem

            anchors { left: parent.left; right: parent.right }

            title: model.title
            content: model.content
            date: model.date
            read: model.read
            feedIcon: settings.viewMode==1 || settings.viewMode==3 || settings.viewMode==4 || settings.viewMode==5 ? model.feedIcon : ""
            author: model.author
            image: model.image
            readlater: model.readlater
            index: model.index
            cached: model.cached
            fresh: model.fresh
            //showMarkedAsRead: settings.viewMode!=4 && model.read<2
            objectName: "EntryDelegate"

            onHolded: {
                contextMenu.openMenu(model.index, model.read, model.readlater);
            }

            Component.onCompleted: {
                // Dynamic creation of new items if last item is compleated
                if (model.index==entryModel.count()-1) {
                    //console.log(index);
                    entryModel.createItems(model.index+1,model.index+settings.offsetLimit);
                }
            }

            onClicked: {
                if (timer.running) {
                    // Double click
                    timer.stop();

                    if (model.read==0) {
                        entryModel.setData(model.index, "read", 1);
                    } else {
                        entryModel.setData(model.index, "read", 0);
                    }

                } else {
                    timer.start();
                }
            }

            Timer {
                id: timer
                interval: 400
                onTriggered: {
                    // One click

                    // Not allowed while Syncing
                    if (dm.busy || fetcher.busy || dm.removerBusy) {
                        notification.show(qsTr("Please wait until current task is complete."));
                        return;
                    }

                    // Entry not cached and offline mode enabled
                    if (settings.offlineMode && !model.cached) {
                        notification.show(qsTr("Offline version not available."));
                        return;
                    }

                    // Switch to Offline mode if no network
                    if (!settings.offlineMode && !dm.online) {
                        if (model.cached) {
                            // Entry cached
                            notification.show(qsTr("Network connection is unavailable.\nSwitching to Offline mode."));
                            settings.offlineMode = true;
                        } else {
                            // Entry not cached
                            notification.show(qsTr("Network connection is unavailable."));
                            return;
                        }
                    }

                    expanded = false;
                    var onlineUrl = model.link;
                    var offlineUrl = cache.getUrlbyId(model.uid);
                    pageStack.push(Qt.resolvedUrl("WebPreviewPage.qml"),
                                   {"entryId": model.uid,
                                       "onlineUrl": onlineUrl,
                                       "offlineUrl": offlineUrl,
                                       "title": model.title,
                                       "stared": model.readlater==1,
                                       "index": model.index,
                                       "feedindex": root.index,
                                       "read" : model.read==1,
                                       "cached" : model.cached
                                   });

                }
            }

            onExpandedChanged: {
                // Collapsing all other items on expand
                if (expanded) {
                    for (var i = 0; i < listView.contentItem.children.length; i++) {
                        var curItem = listView.contentItem.children[i];
                        //console.log(curItem);
                        if (curItem !== listItem) {
                            if (curItem.objectName==="EntryDelegate")
                                curItem.expanded = false;
                        }
                    }
                    listView.positionViewAtIndex(model.index, ListView.Visible);
                    //listView.positionViewAtIndex(model.index, ListView.Beginning);
                }
            }

        }

        ViewPlaceholder {
            enabled: listView.count == 0
            text: settings.viewMode==4 ? qsTr("No saved items") :
                  settings.showOnlyUnread ? qsTr("No unread items") :
                                            qsTr("No items")
        }
    }

    ContextMenu {
        id: contextMenu
        property int index
        property int read
        property int readlater

        function openMenu(i, r, rl) {

            if (settings.viewMode!=4) {
                var unread = entryModel.countUnread();
                if (unread>0) {
                    readAllMenuItem.enabled = true;
                } else {
                    readAllMenuItem.enabled = false;
                }

                if (read<2) {
                    readMenuItem.enabled = true;
                } else {
                    readMenuItem.enabled = false;
                }
            } else {
                readMenuItem.enabled = false;
                readAllMenuItem.enabled = false;
            }

            index = i;
            read = r;
            readlater = rl;

            open();
        }

        onStatusChanged: {
            if (progressPanelDm.open) {
                if (status===DialogStatus.Opening) {
                    progressPanelDm.visible = false;
                }
                if (status===DialogStatus.Closed) {
                    progressPanelDm.visible = true;
                }
            }
        }

        MenuLayout {
            MenuItem {
                text: contextMenu.readlater ? qsTr("Unsave") : qsTr("Save")
                onClicked: {
                    if (contextMenu.readlater) {
                        entryModel.setData(contextMenu.index, "readlater", 0);
                    } else {
                        entryModel.setData(contextMenu.index, "readlater", 1);
                    }
                }
            }
            MenuItem {
                id: readMenuItem
                text: contextMenu.read ? qsTr("Mark as unread") : qsTr("Mark as read")
                onClicked: {
                    if (contextMenu.read) {
                        entryModel.setData(contextMenu.index, "read", 0);
                    } else {
                        entryModel.setData(contextMenu.index, "read", 1);
                    }
                }
            }
            MenuItem {
                id: readAllMenuItem
                text: contextMenu.read ? qsTr("Mark all as unread") : qsTr("Mark all as read")
                onClicked: {
                    if (settings.viewMode==1 ||
                            settings.viewMode==3 ||
                            settings.viewMode==4 ||
                            settings.viewMode==5) {
                        readAllDialog.open();
                    } else {
                        entryModel.setAllAsRead();
                    }
                }
            }
        }
    }

    ReadAllDialog {
        id: readAllDialog
    }

    ScrollDecorator { flickableItem: listView }
}
