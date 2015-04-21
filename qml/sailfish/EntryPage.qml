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

import QtQuick 2.0
import Sailfish.Silica 1.0


Page {
    id: root

    allowedOrientations: {
        switch (settings.allowedOrientations) {
        case 1:
            return Orientation.Portrait;
        case 2:
            return Orientation.Landscape;
        }
        return Orientation.Landscape | Orientation.Portrait;
    }

    property bool showBar: true
    property string title
    property int index

    ActiveDetector {
        onInit: { bar.flick = listView;}
    }

    RemorsePopup {id: remorse}

    SilicaListView {
        id: listView
        model: entryModel

        anchors { top: parent.top; left: parent.left; right: parent.right }
        clip:true

        height: app.flickHeight

        PageMenu {
            id: menu
            showAbout: settings.viewMode==3 || settings.viewMode==4 || settings.viewMode==5 ? true : false
            showMarkAsRead: false
            showMarkAsUnread: false
            showShowOnlyUnread: true

            onMarkedAsRead: {
                if (settings.viewMode==1 || settings.viewMode==5) {
                    remorse.execute(qsTr("Marking articles as read"), function(){entryModel.setAllAsRead()});
                    return;
                }
                if (settings.viewMode==3) {
                    remorse.execute(qsTr("Marking all your articles as read"), function(){entryModel.setAllAsRead()});
                    return;
                }
                if (settings.viewMode==4) {
                    remorse.execute(qsTr("Marking all saved articles as read"), function(){entryModel.setAllAsRead()});
                    return;
                }

                entryModel.setAllAsRead();
            }

            /*onMarkedAsUnread:  {
                if (settings.viewMode==1 ||
                        settings.viewMode==3 ||
                        settings.viewMode==4 ||
                        settings.viewMode==5) {
                    pageStack.push(Qt.resolvedUrl("UnreadAllDialog.qml"),{"type": 2});
                } else {
                    entryModel.setAllAsUnread();
                }
            }*/

            onActiveChanged: {
                if (active) {
                    if (settings.viewMode!=4) {
                        showMarkAsRead = entryModel.countUnread()!=0;
                        /*if (!settings.showOnlyUnread)
                            showMarkAsUnread = entryModel.countRead()!=0;*/
                    }
                }
            }
        }

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

        delegate: EntryDelegate {
            id: delegate
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
            last: model.uid=="last"
            daterow: model.uid=="daterow"
            showMarkedAsRead: settings.viewMode!=4 && model.read<2
            objectName: "EntryDelegate"

            Component.onCompleted: {
                //console.log(image);
                // Dynamic creation of new items if last item is compleated
                if (index==entryModel.count()-1) {
                    //console.log(index);
                    entryModel.createItems(index+1,index+settings.offsetLimit);
                }
            }

            onClicked: {
                if (timer.running) {
                    // Double click
                    timer.stop();

                    if (model.read==0) {
                        entryModel.setData(model.index, "read", 1);
                        //read = 1;
                    } else {
                        entryModel.setData(model.index, "read", 0);
                        //read = 0;
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

                    /*console.log("date: "+model.date);
                    console.log("read: "+model.read);
                    console.log("readlater: "+model.readlater);
                    console.log("image: "+model.image);
                    console.log("feedIcon: "+feedIcon+" model.feedIcon: "+model.feedIcon);
                    console.log("showMarkedAsRead: "+showMarkedAsRead);*/

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
                        if (curItem !== delegate) {
                            if (curItem.objectName==="EntryDelegate")
                                curItem.expanded = false;
                        }
                    }
                    listView.positionViewAtIndex(model.index, ListView.Visible);
                }
            }

            onMarkedAsRead: {
                entryModel.setData(model.index, "read", 1);
            }

            onMarkedAsUnread: {
                entryModel.setData(model.index, "read", 0);
            }

            onMarkedReadlater: {
                entryModel.setData(index, "readlater", 1);
            }

            onUnmarkedReadlater: {
                entryModel.setData(index, "readlater", 0);
            }
        }

        ViewPlaceholder {
            id: placeholder
            enabled: listView.count == 0
            text: fetcher.busy ? qsTr("Wait until Sync finish.") :
                      settings.viewMode==4 ? qsTr("No saved items") :
                      settings.showOnlyUnread ? qsTr("No unread items") : qsTr("No items")
        }

        Component.onCompleted: {
            if (listView.count == 0 && settings.viewMode==4)
                bar.open();
        }

        VerticalScrollDecorator {
            flickable: listView
        }

    }
}
