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

    ActiveDetector {
        onActivated: {
            feedModel.updateFlags();
        }
    }

    SilicaListView {
        id: listView
        model: feedModel

        anchors { top: parent.top; left: parent.left; right: parent.right }
        clip:true

        height: {
            /*if ((dm.busy||fetcher.busy) && bar.open)
                return isPortrait ? app.height-Theme.itemSizeMedium : app.width-1.6*Theme.itemSizeMedium;*/
            if (dm.busy||fetcher.busy)
                return isPortrait ? app.height-Theme.itemSizeMedium : app.width-0.8*Theme.itemSizeMedium;
            /*if (bar.open)
                return isPortrait ? app.height-Theme.itemSizeMedium : app.width-0.8*Theme.itemSizeMedium;*/
            return isPortrait ? app.height : app.width;
        }

        PageMenu {
            id: menu
            showAbout: settings.viewMode==2 ? true : false
            showMarkAsRead: false
            showMarkAsUnread: false

            onMarkedAsRead: feedModel.setAllAsRead()
            onMarkedAsUnread: feedModel.setAllAsUnread()

            onActiveChanged: {
                if (active) {
                    showMarkAsRead = feedModel.countUnread()!=0;
                    showMarkAsUnread = !showMarkAsRead
                }
            }
        }

        header: PageHeader {
            title: settings.viewMode==2 ? qsTr("Feeds") : root.title
        }

        delegate: ListItem {
            id: listItem
            contentHeight: item.height + 2 * Theme.paddingMedium

            Rectangle {
                anchors.top: parent.top; anchors.left: parent.left
                width: Theme.paddingSmall; height: item.height
                visible: model.fresh
                radius: 10

                gradient: Gradient {
                    GradientStop { position: 0.0; color: Theme.rgba(Theme.highlightColor, 0.4) }
                    GradientStop { position: 1.0; color: Theme.rgba(Theme.highlightColor, 0.0) }
                }
            }

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
                width: visible ? Theme.iconSizeSmall : 0
                height: width
                anchors.left: parent.left; anchors.leftMargin: Theme.paddingLarge
                anchors.top: item.top; anchors.topMargin: Theme.paddingSmall
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
                pageStack.push(Qt.resolvedUrl("EntryPage.qml"),{"title": title, "index": model.index, "readlater": false});
            }

            menu: ContextMenu {
                MenuItem {
                    text: qsTr("Mark all as read")
                    enabled: model.unread!=0
                    visible: enabled
                    onClicked: {
                        feedModel.markAsRead(model.index);
                    }
                }
                MenuItem {
                    text: qsTr("Mark all as unread")
                    enabled: model.read!=0
                    visible: enabled
                    onClicked: {
                        feedModel.markAsUnread(model.index);
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
