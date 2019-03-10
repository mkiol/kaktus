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

    objectName: "tabs"

    property bool showBar: true
    property alias remorse: _remorse

    allowedOrientations: {
        switch (settings.allowedOrientations) {
        case 1:
            return Orientation.Portrait
        case 2:
            return Orientation.Landscape
        }
        return Orientation.Landscape | Orientation.Portrait
    }

    ActiveDetector {
        onActivated: {
            tabModel.updateFlags()
            bar.flick = listView
        }
        onInit: {
            bar.flick = listView
        }
    }

    RemorsePopup {
        id: _remorse
    }

    SilicaListView {
        id: listView
        model: tabModel

        anchors { top: parent.top; left: parent.left; right: parent.right }
        height: app.flickHeight
        Behavior on height {NumberAnimation { duration: 200; easing.type: Easing.OutQuad }}
        clip: true

        PageMenu {
            id: menu
            showAbout: true
        }

        header: PageHeader {
            title: settings.signinType < 10 ? qsTr("Tabs") : qsTr("Folders")
        }

        delegate: SimpleListItem {
            property bool last: model.uid === "last"

            small: true
            showPlaceholder: true
            title: model.uid === "subscriptions" ? qsTr("Subscriptions") :
                   model.uid === "friends" ? qsTr("Following") :
                   model.uid === "global.uncategorized" ? qsTr("Uncategorized") :
                   model.title
            icon: model.uid === "friends" ? "image://icons/icon-m-friend" :
                  model.iconUrl.length > 0 ? iconUrl : ""
            enabled: !last
            unreadCount: enabled ? model.unread : 0

            menu: ContextMenu {
                MenuItem {
                    id: readItem
                    text: qsTr("Mark all as read")
                    enabled: model.unread !== 0
                    visible: enabled
                    onClicked: {
                        tabModel.markAsRead(model.index)
                    }
                }
                MenuItem {
                    id: unreadItem
                    text: qsTr("Mark all as unread")
                    enabled: model.read !== 0 && settings.signinType < 10 // Only Netvibes
                    visible: enabled
                    onClicked: {
                        tabModel.markAsUnread(model.index)
                    }
                }
            }

            openMenuOnPressAndHold: enabled && (readItem.enabled || unreadItem.enabled)

            onClicked: {
                if (enabled) {
                    var vm = settings.viewMode
                    if (vm == 0) {
                        utils.setFeedModel(uid);
                        pageStack.push(Qt.resolvedUrl("FeedPage.qml"),
                                       {"title": title.text, "index": model.index})
                    } else if (vm == 1) {
                        utils.setEntryModel(uid);
                        pageStack.push(Qt.resolvedUrl("EntryPage.qml"),
                                       {"title": title.text, "readlater": false})
                    }
                }
            }
        }

        ViewPlaceholder {
            id: placeholder
            enabled: listView.count < 1
            text: fetcher.busy ? qsTr("Wait until sync finish") :
                  settings.signinType < 10 ? qsTr("No tabs") : qsTr("No folders")
        }

        VerticalScrollDecorator {
            flickable: listView
        }
    }
}
