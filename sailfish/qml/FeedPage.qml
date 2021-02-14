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
            title: settings.viewMode == 2 ? qsTr("Feeds") : root.title
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
                                   {"title": title.text, "index": model.index, "readlater": false})
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
