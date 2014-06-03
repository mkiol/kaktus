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

    PageHeader {
        id: header
        title: qsTr("Tabs")
    }

    ListView {
        id: listView

        model: tabModel

        anchors { top: header.bottom; left: parent.left; right: parent.right; bottom: parent.bottom }

        clip: true

        delegate: ListDelegate {
            id: listItem

            property bool readlaterItem: model.uid==="readlater"

            iconSize: Theme.iconSizeSmall
            iconVisible: settings.showTabIcons && !readlaterItem
            titleText: model.title
            iconSource: readlaterItem ? "image://theme/icon-m-toolbar-favorite-mark" : ""
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
        anchors.horizontalCenter: parent.horizontalCenter
    }

    ScrollDecorator { flickableItem: listView }
}
