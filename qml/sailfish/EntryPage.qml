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

    property string title
    property int index
    property int read
    property int unread
    property bool readlater

    SilicaListView {
        id: listView
        model: entryModel

        anchors { top: parent.top; left: parent.left; right: parent.right }
        clip:true

        height: {
            if (dm.busy||fetcher.busy)
                return isPortrait ? app.height-Theme.itemSizeMedium : app.width-0.8*Theme.itemSizeMedium;
            return isPortrait ? app.height : app.width;
        }

        PageMenu {
            showAbout: false
            showMarkAsRead: root.readlater ? false : root.unread!=0
            showMarkAsUnread: false

            onMarkedAsRead:    {
                feedModel.markAllAsRead(root.index);
                tabModel.updateFlags();
                entryModel.setAllAsRead();
            }

            onMarkedAsUnread: {
                feedModel.markAllAsUnread(root.index);
                tabModel.updateFlags();
                entryModel.setAllAsUnread();
            }

            onActiveChanged: {
                if (active) {
                    root.read = entryModel.countRead();
                    root.unread = entryModel.countUnread();
                }
            }
        }

        header: PageHeader {
            title: root.title
        }

        delegate: EntryDelegate {
            id: delegate
            title: model.title
            content: model.content
            date: model.date
            read: model.read
            author: model.author
            image: model.image
            readlater: model.readlater
            index: model.index
            cached: model.cached
            showMarkedAsRead: !root.readlater

            onClicked: {

                // Not allowed while Syncing
                if (dm.busy || fetcher.busy) {
                    notification.show(qsTr("Please wait until Sync finishes"));
                    return;
                }

                // Entry not cached and offline mode enabled
                if (settings.offlineMode && !model.cached) {
                    /*pageStack.push(Qt.resolvedUrl("NoContentPage.qml"),
                                   {"title": model.title,});*/
                    notification.show(qsTr("Offline version not available"));
                    return;
                }

                // Switch to Offline mode if no network
                if (!settings.offlineMode && !dm.online) {
                    if (model.cached) {
                        // Entry cached
                        notification.show(qsTr("Network connection is unavailable\nSwitching to Offline mode"));
                        settings.offlineMode = true;
                    } else {
                        // Entry not cached
                        notification.show(qsTr("Network connection is unavailable"));
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
                                   "stared": model.readlater===1,
                                   "index": model.index,
                                   "feedindex": root.index,
                                   "read" : model.read===1,
                                   "cached" : model.cached
                               });
            }

            onMarkedAsRead: {
                entryModel.setData(model.index, "read", 1);
                if (!readlater)
                    feedModel.decrementUnread(root.index);
                tabModel.updateFlags();
            }

            onMarkedAsUnread: {
                entryModel.setData(model.index, "read", 0);
                if (!readlater)
                    feedModel.incrementUnread(root.index);
                tabModel.updateFlags();
            }
        }

        ViewPlaceholder {
            enabled: listView.count == 0
            text: settings.showOnlyUnread ? qsTr("No unread items") : qsTr("No items")
        }

        VerticalScrollDecorator {
            flickable: listView
        }

    }
}
