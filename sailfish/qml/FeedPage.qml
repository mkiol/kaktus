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
            return Orientation.Portrait;
        case 2:
            return Orientation.Landscape;
        }
        return Orientation.Landscape | Orientation.Portrait;
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
        clip:true

        height: app.flickHeight

        PageMenu {
            id: menu
            showAbout: true
        }

        header: PageHeader {
            title: settings.viewMode==2 ? qsTr("Feeds") : root.title
        }

        delegate: ListItem {
            id: listItem

            property bool last: model.uid==="last"
            property bool defaultIcon: model.icon === "http://s.theoldreader.com/icons/user_icon.png"

            enabled: !last
            contentHeight: last ? app.stdHeight : Math.max(item.height, image.height) + 2 * Theme.paddingMedium

            Rectangle {
                anchors.top: parent.top; anchors.right: parent.right
                width: Theme.paddingSmall; height: item.height
                visible: model.fresh && !listItem.last
                radius: 10

                gradient: Gradient {
                    GradientStop { position: 0.0; color: Theme.rgba(Theme.highlightColor, 0.4) }
                    GradientStop { position: 1.0; color: Theme.rgba(Theme.highlightColor, 0.0) }
                }
            }

            FeedIcon {
                id: image
                visible: !listItem.last
                y: Theme.paddingMedium
                anchors.left: parent.left
                showPlaceholder: true
                showBackground: !listItem.defaultIcon
                source: listItem.defaultIcon ? "image://icons/icon-m-friend" : model.icon
                text: model.title
                width: visible ? 1.2*Theme.iconSizeSmall : 0
                height: width
            }

            Label {
                id: item
                visible: !listItem.last
                wrapMode: Text.AlignLeft
                y: Theme.paddingMedium
                anchors {
                    left: image.visible ? image.right : parent.left
                    right: unreadbox.visible ? unreadbox.left : parent.right
                    verticalCenter: parent.verticalCenter
                    leftMargin: Theme.paddingLarge
                    rightMargin: Theme.paddingLarge
                }
                font.pixelSize: Theme.fontSizeMedium
                text: model.title
                color: listItem.down ?
                        (model.unread ? Theme.highlightColor : Theme.secondaryHighlightColor) :
                        (model.unread ? Theme.primaryColor : Theme.secondaryColor)
            }

            /*Column {
                id: item
                spacing: 0.5*Theme.paddingSmall
                anchors.verticalCenter: parent.verticalCenter
                //anchors.left: image.visible ? image.right : Theme.fontSizeMedium
                anchors.right: unreadbox.visible ? unreadbox.left : parent.right
                visible: !listItem.last
            }*/

            Rectangle {
                id: unreadbox
                y: Theme.paddingSmall
                anchors {
                    right: parent.right
                    rightMargin: Theme.paddingLarge
                }
                width: unreadlabel.width + 2 * Theme.paddingSmall
                height: unreadlabel.height + 2 * Theme.paddingSmall
                color: Theme.rgba(Theme.highlightBackgroundColor, 0.2)
                radius: 5
                visible: model.unread!==0 && !listItem.last

                Label {
                    id: unreadlabel
                    anchors.centerIn: parent
                    text: model.unread
                    color: Theme.highlightColor
                }
            }

            onClicked: {
                if (!listItem.last) {
                    utils.setEntryModel(uid);
                    pageStack.push(Qt.resolvedUrl("EntryPage.qml"),{"title": title, "index": model.index, "readlater": false});
                }
            }

            showMenuOnPressAndHold: !listItem.last && (readItem.enabled || unreadItem.enabled)

            menu: ContextMenu {
                id: contextMenu
                MenuItem {
                    id: readItem
                    text: qsTr("Mark all as read")
                    enabled: model.unread!==0
                    visible: enabled
                    onClicked: {
                        feedModel.markAsRead(model.index);
                    }
                }
                MenuItem {
                    id: unreadItem
                    text: qsTr("Mark all as unread")
                    enabled: model.read!==0 && settings.signinType<10
                    visible: enabled
                    onClicked: {
                        feedModel.markAsUnread(model.index);
                    }
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
