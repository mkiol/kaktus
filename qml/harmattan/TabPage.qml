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

    /*PageHeader {
        id: header
        title: qsTr("Tabs")
    }*/

    ListView {
        id: listView

        model: tabModel

        //anchors { top: header.bottom; left: parent.left; right: parent.right; bottom: parent.bottom }
        anchors.fill: parent

        clip: true

        PullBar {}

        delegate: ListDelegate {
            id: listItem

            property bool readlaterItem: model.uid==="readlater"

            iconSize: Theme.iconSizeSmall
            iconVisible: settings.showTabIcons && !readlaterItem
            titleText: model.title
            iconSource: readlaterItem ? "image://theme/icon-m-toolbar-favorite-mark-white" : ""
            unread: model.unread
            showUnread: true
            titleColor: model.unread>0 || readlaterItem ? Theme.primaryColor : Theme.secondaryColor

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

            onClicked: {
                if (readlaterItem) {
                    utils.setEntryModel(uid);
                    pageStack.push(Qt.resolvedUrl("EntryPage.qml"),{"title": model.title, "index": model.index});
                } else {
                    utils.setFeedModel(uid);
                    pageStack.push(Qt.resolvedUrl("FeedPage.qml"),{"title": model.title});
                }
            }

            onHolded: !readlaterItem ? contextMenu.openMenu(model.index, model.read, model.unread) : {}

            Connections {
                target: settings
                onShowTabIconsChanged: {
                    if (!readlaterItem) {
                        if (settings.showTabIcons && iconUrl!=="")
                            iconSource = cache.getUrlbyUrl(iconUrl);
                        else
                            iconSource = "";
                    }
                }
            }

            Component.onCompleted: {
                if (!readlaterItem) {
                    if (settings.showTabIcons && iconUrl!=="")
                        iconSource = cache.getUrlbyUrl(iconUrl);
                    else
                        iconSource = "";
                }
            }

            /*Dialog {
                id: contextMenu
                buttons: Column {
                    spacing: UiConstants.DefaultMargin

                    Button {
                        text: qsTr("Mark all as read")
                        enabled: model.unread!=0
                        visible: enabled
                        onClicked: {
                            tabModel.markAllAsRead(model.index);
                            contextMenu.accept();
                        }
                    }

                    Button {
                        text: qsTr("Mark all as unread")
                        enabled: model.read!=0
                        visible: enabled
                        onClicked: {
                            tabModel.markAllAsUnread(model.index);
                            contextMenu.accept();
                        }
                    }
                }
            }*/
        }
    }

    ContextMenu {
        id: contextMenu
        property int index
        property int read
        property int unread

        function openMenu(i, r, u) {
            index = i;
            read = r;
            unread = u;
            open();
        }

        MenuLayout {
            MenuItem {
                text: qsTr("Mark all as read")
                enabled: contextMenu.unread!=0
                //visible: enabled
                onClicked: {
                    tabModel.markAsRead(contextMenu.index);
                }
            }
            MenuItem {
                text: qsTr("Mark all as unread")
                enabled: contextMenu.read!=0
                //visible: enabled
                onClicked: {
                    tabModel.markAsUnread(contextMenu.index);
                }
            }
        }
    }

    ViewPlaceholder {
        enabled: listView.count == 1
        text: qsTr("No tabs")
        secondaryText: fetcher.busy ? qsTr("Wait until Sync finish") : qsTr("Press button to do first Sync")
    }

    Image {
        visible: listView.count == 1 && !fetcher.busy && !dm.busy
        source: "arrow.png"
        anchors.bottom: parent.bottom; anchors.bottomMargin: Theme.paddingLarge
        x: (parent.width/3)-UiConstants.DefaultMargin;
    }

    ScrollDecorator { flickableItem: listView }
}
