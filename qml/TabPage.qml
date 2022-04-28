/* Copyright (C) 2014-2022 Michal Kosciesza <michal@mkiol.net>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

import QtQuick 2.0
import Sailfish.Silica 1.0

import harbour.kaktus.Settings 1.0

Page {
    id: root

    objectName: "tabs"

    property bool showBar: true
    property alias remorse: _remorse

    allowedOrientations: {
        switch (settings.allowedOrientations) {
        case 1:
            return Orientation.PortraitMask;
        case 2:
            return Orientation.LandscapeMask;
        }
        return Orientation.All;
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
                    if (settings.viewMode === Settings.TabsFeedsEntries) {
                        utils.setFeedModel(uid);
                        pageStack.push(Qt.resolvedUrl("FeedPage.qml"),
                                       {"title": title, "index": model.index})
                    } else if (settings.viewMode === Settings.TabsEntries) {
                        utils.setEntryModel(uid);
                        console.log()
                        pageStack.push(Qt.resolvedUrl("EntryPage.qml"),
                                       {"title": title})
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
