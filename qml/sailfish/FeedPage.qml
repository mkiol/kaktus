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

    SilicaListView {
        id: listView
        model: feedModel

        anchors { top: parent.top; left: parent.left; right: parent.right }
        clip:true

        height: {
            if (dm.busy||fetcher.busy)
                return isPortrait ? app.height-Theme.itemSizeMedium : app.width-0.8*Theme.itemSizeMedium;
            return isPortrait ? app.height : app.width;
        }

        MainMenu{}

        header: PageHeader {
            title: root.title
        }

        delegate: ListItem {
            id: listItem
            contentHeight: item.height + 2 * Theme.paddingMedium

            Column {
                id: item
                spacing: 0.5*Theme.paddingSmall
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: image.visible ? image.right : parent.left
                anchors.right: unreadbox.visible ? unreadbox.left : parent.right

                Label {
                    wrapMode: Text.AlignLeft
                    anchors.left: parent.left; anchors.right: parent.right;
                    anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
                    font.pixelSize: Theme.fontSizeMedium
                    text: title
                    color: listItem.down ?
                               (model.unread ? Theme.highlightColor : Theme.secondaryHighlightColor) :
                               (model.unread ? Theme.primaryColor : Theme.secondaryColor)
                }
            }

            Rectangle {
                id: unreadbox
                anchors.right: parent.right; anchors.rightMargin: Theme.paddingLarge
                anchors.verticalCenter: parent.verticalCenter
                width: unreadlabel.width + 2 * Theme.paddingSmall
                height: unreadlabel.height + 2 * Theme.paddingSmall
                color: Theme.rgba(Theme.highlightBackgroundColor, 0.2)
                radius: 5
                visible: model.unread!=0

                Label {
                    id: unreadlabel
                    anchors.centerIn: parent
                    text: model.unread
                    //color: listItem.down ? Theme.secondaryHighlightColor : Theme.secondaryColor
                    color: Theme.highlightColor
                }

            }

            Image {
                id: image
                width: Theme.iconSizeSmall
                height: Theme.iconSizeSmall
                anchors.left: parent.left; anchors.leftMargin: Theme.paddingLarge
                anchors.verticalCenter: parent.verticalCenter
                visible: status!=Image.Error && status!=Image.Null && settings.showTabIcons
            }

            Connections {
                target: settings
                onShowTabIconsChanged: {
                    if (settings.showTabIcons && model.icon!="")
                        image.source = cache.getUrlbyUrl(model.icon);
                    else
                        image.source = "";
                }
            }

            Component.onCompleted: {
                if (settings.showTabIcons && model.icon!="")
                    image.source = cache.getUrlbyUrl(model.icon);
                else
                    image.source = "";
            }

            onClicked: {
                utils.setEntryModel(uid);
                pageStack.push(Qt.resolvedUrl("EntryPage.qml"),{"title": title, "index": model.index});
            }

            menu: ContextMenu {
                MenuItem {
                    text: qsTr("Mark all as read")
                    enabled: model.unread!=0
                    visible: enabled
                    onClicked: {
                        feedModel.markAllAsRead(model.index);
                        tabModel.updateFlags();
                    }
                }
                MenuItem {
                    text: qsTr("Mark all as unread")
                    enabled: model.read!=0
                    visible: enabled
                    onClicked: {
                        feedModel.markAllAsUnread(model.index);
                        tabModel.updateFlags();
                    }
                }
            }

        }

        ViewPlaceholder {
            enabled: listView.count == 0
            text: qsTr("No feeds")
        }

        VerticalScrollDecorator {
            flickable: listView
        }

    }

}
