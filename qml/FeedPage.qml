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

    objectName: "feeds"

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

    property string title
    property int index

    ActiveDetector {
        onActivated: {
            feedModel.updateFlags()
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
        model: feedModel

        anchors { top: parent.top; left: parent.left; right: parent.right }
        height: app.flickHeight
        Behavior on height {NumberAnimation { duration: 200; easing.type: Easing.OutQuad }}
        clip: true

        PageMenu {
            id: menu
            showAbout: true
        }

        header: PageHeader {
            title: settings.viewMode === Settings.FeedsEntries ? qsTr("Feeds") : root.title
        }

        delegate: SimpleListItem {
            property bool last: model.uid === "last"
            property bool defaultIcon: model.icon === "http://s.theoldreader.com/icons/user_icon.png"

            showPlaceholder: true
            small: true
            title: model.title
            icon: defaultIcon ? "image://icons/icon-m-friend" : model.icon
            enabled: !last
            unreadCount: enabled ? model.unread : 0

            menu: ContextMenu {
                MenuItem {
                    id: readItem
                    text: qsTr("Mark all as read")
                    enabled: model.unread !== 0
                    visible: enabled
                    onClicked: {
                        feedModel.markAsRead(model.index)
                    }
                }
                MenuItem {
                    id: unreadItem
                    text: qsTr("Mark all as unread")
                    enabled: model.read !== 0 && settings.signinType < 10 // Only Netvibes
                    visible: enabled
                    onClicked: {
                        feedModel.markAsUnread(model.index)
                    }
                }
            }

            openMenuOnPressAndHold: enabled && (readItem.enabled || unreadItem.enabled)

            onClicked: {
                if (enabled) {
                    utils.setEntryModel(uid);
                    pageStack.push(Qt.resolvedUrl("EntryPage.qml"),
                                   {"title": title, "index": model.index})
                }
            }
        }

        ViewPlaceholder {
            id: placeholder
            enabled: listView.count == 0
            text: fetcher.busy ? qsTr("Wait until sync finish") : qsTr("No feeds")
        }

        VerticalScrollDecorator {
            flickable: listView
        }
    }
}
