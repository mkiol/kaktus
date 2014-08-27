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

    //property int read
    //property int unread

    ActiveDetector {
        onActivated: {
            tabModel.updateFlags();
        }
    }

    SilicaListView {
        id: listView
        model: tabModel

        anchors { top: parent.top; left: parent.left; right: parent.right }

        height: {
            /*if ((dm.busy||fetcher.busy) && bar.open)
                return isPortrait ? app.height-Theme.itemSizeMedium : app.width-1.6*Theme.itemSizeMedium;*/
            if (dm.busy||fetcher.busy)
                return isPortrait ? app.height-Theme.itemSizeMedium : app.width-0.8*Theme.itemSizeMedium;
            /*if (bar.open)
                return isPortrait ? app.height-Theme.itemSizeMedium : app.width-0.8*Theme.itemSizeMedium;*/
            return isPortrait ? app.height : app.width;
        }

        clip:true

        PageMenu {
            id: menu
            showMarkAsRead: false
            showMarkAsUnread: false
        }

        header: PageHeader {
            title: qsTr("Tabs")
        }

        delegate: Item {

            property bool readlaterItem: model.uid==="readlater"

            anchors.left: parent.left; anchors.right: parent.right
            height: readlaterItem ? listItem.height+Theme.paddingMedium : listItem.height

            ListItem {
                id: listItem

                anchors.top: parent.top

                contentHeight: {
                    if (visible) {
                        if (readlaterItem)
                            return itemReadlater.height + 2 * Theme.paddingMedium;
                        else
                            return item.height + 2 * Theme.paddingMedium;
                    } else {
                        return 0;
                    }
                }

                visible: {
                    if (readlaterItem) {
                        if (listView.count==1)
                            return false;
                        if (settings.showStarredTab)
                            return true
                        return false;
                    }
                    return true;
                }

                Column {
                    id: item

                    visible: !readlaterItem
                    spacing: 0.5*Theme.paddingSmall
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: image.visible ? image.right : parent.left
                    anchors.right: unreadbox.visible ? unreadbox.left : parent.right

                    Label {
                        wrapMode: Text.AlignLeft
                        anchors.left: parent.left; anchors.right: parent.right;
                        anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
                        font.pixelSize: Theme.fontSizeMedium
                        color: listItem.down ?
                                   (model.unread ? Theme.highlightColor : Theme.secondaryHighlightColor) :
                                   (model.unread ? Theme.primaryColor : Theme.secondaryColor)
                        text: title
                    }
                }

                Rectangle {
                    id: unreadbox
                    anchors.right: parent.right; anchors.rightMargin: Theme.paddingLarge
                    anchors.verticalCenter: parent.verticalCenter
                    width: unreadlabel.width + 3 * Theme.paddingSmall
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
                    visible: status!=Image.Error && status!=Image.Null && settings.showTabIcons && !readlaterItem
                }

                // readlater

                Column {
                    id: itemReadlater
                    spacing: 1.0 * Theme.paddingSmall
                    anchors.left: star.right; anchors.right: parent.right
                    //anchors.top: parent.top
                    anchors.verticalCenter: parent.verticalCenter
                    visible: readlaterItem

                    Label {
                        wrapMode: Text.AlignLeft
                        anchors.left: parent.left; anchors.right: parent.right;
                        anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
                        font.pixelSize: Theme.fontSizeMedium
                        color: listItem.down ? Theme.highlightColor : Theme.primaryColor
                        text: title
                    }
                }

                Image {
                    id: star
                    width: Theme.iconSizeSmall
                    height: Theme.iconSizeSmall
                    anchors.left: parent.left; anchors.leftMargin: Theme.paddingLarge
                    anchors.verticalCenter: itemReadlater.verticalCenter
                    visible: readlaterItem
                    source: listItem.down ? "image://theme/icon-m-favorite-selected?"+Theme.highlightColor :
                                            "image://theme/icon-m-favorite-selected"
                }

                // --

                Connections {
                    target: settings
                    onShowTabIconsChanged: {
                        if (settings.showTabIcons && !readlaterItem && iconUrl!="")
                            image.source = cache.getUrlbyUrl(iconUrl);
                        else
                            image.source = "";
                    }
                }

                Component.onCompleted: {
                    if (settings.showTabIcons && !readlaterItem && iconUrl!="") {
                        image.source = cache.getUrlbyUrl(iconUrl);
                    } else {
                        image.source = "";
                    }
                }

                onClicked: {
                    if (readlaterItem) {
                        utils.setEntryModel(uid);
                        pageStack.push(Qt.resolvedUrl("EntryPage.qml"),{"title": title, "readlater": true});
                    } else {
                        if (settings.viewMode == 0) {
                            utils.setFeedModel(uid);
                            pageStack.push(Qt.resolvedUrl("FeedPage.qml"),{"title": title, "index": model.index});
                        }
                        if (settings.viewMode == 1) {
                            utils.setEntryModel(uid);
                            pageStack.push(Qt.resolvedUrl("EntryPage.qml"),{"title": title, "readlater": false});
                        }
                    }
                }

                showMenuOnPressAndHold: !readlaterItem

                menu: ContextMenu {

                    MenuItem {
                        text: qsTr("Mark all as read")
                        enabled: model.unread!=0
                        visible: enabled
                        onClicked: {
                            tabModel.markAsRead(model.index);
                        }
                    }
                    MenuItem {
                        text: qsTr("Mark all as unread")
                        enabled: model.read!=0
                        visible: enabled
                        onClicked: {
                            tabModel.markAsUnread(model.index);
                        }
                    }
                }
            }
        }

        ViewPlaceholder {
            enabled: listView.count == 1
            text: qsTr("No tabs")

            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.secondaryHighlightColor
                text: fetcher.busy ? qsTr("Wait until Sync finish") : qsTr("Pull down to do first Sync")
            }
        }

        VerticalScrollDecorator {
            flickable: listView
        }

    }
}
