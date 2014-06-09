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

    tools: MainToolbar {}

    orientationLock: {
        switch (settings.allowedOrientations) {
        case 1:
            return PageOrientation.LockPortrait;
        case 2:
            return PageOrientation.LockLandscape;
        }
        return PageOrientation.Automatic;
    }

    PageHeader {
        id: header
        title: root.title
    }

    ListView {
        id: listView

        model: entryModel

        anchors { top: header.bottom; left: parent.left; right: parent.right; bottom: parent.bottom }

        clip: true

        delegate: EntryDelegate {

            id: listItem

            anchors { left: parent.left; right: parent.right }

            title: model.title
            content: model.content
            date: model.date
            read: model.read
            author: model.author
            readlater: model.readlater
            index: model.index
            cached: model.cached
            feedindex: root.index

            onHolded: contextMenu.open()

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

                //expanded = false;
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

            Dialog {
                id: contextMenu
                buttons: Column {
                    spacing: UiConstants.DefaultMargin

                    Button {
                        text: readlater ? qsTr("Unsave") : qsTr("Save")
                        onClicked: {
                            if (readlater) {
                                entryModel.setData(index, "readlater", 0);
                            } else {
                                entryModel.setData(index, "readlater", 1);
                            }
                            contextMenu.accept()
                        }
                    }

                    Button {
                        text: read ? qsTr("Mark as unread") : qsTr("Mark as read")
                        onClicked: {
                            if (read) {
                                entryModel.setData(index, "read", 0);
                                feedModel.incrementUnread(feedindex);
                            } else {
                                entryModel.setData(index, "read", 1);
                                feedModel.decrementUnread(feedindex);
                                /*if (lblMoreDetails.visible)
                                    root.expanded = false;*/
                            }
                            tabModel.updateFlags();
                            contextMenu.accept()
                        }
                    }
                }
            }

        }

        ViewPlaceholder {
            enabled: listView.count == 0
            text: settings.showOnlyUnread ? qsTr("No unread items") : qsTr("No items")
        }
    }

    ScrollDecorator { flickableItem: listView }
}
