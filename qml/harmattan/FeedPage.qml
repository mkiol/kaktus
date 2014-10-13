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

    property string title
    property int index

    tools: MainToolbar {
        menu: menuItem
    }

    MainMenu {
        id: menuItem
    }

    orientationLock: {
        switch (settings.allowedOrientations) {
        case 1:
            return PageOrientation.LockPortrait;
        case 2:
            return PageOrientation.LockLandscape;
        }
        return PageOrientation.Automatic;
    }

    ActiveDetector {
        onActivated: {
            feedModel.updateFlags();
        }
    }

    ListView {
        id: listView

        model: feedModel

        anchors.fill: parent

        clip: true

        PullBar {}

        header: PageHeader {
            title: settings.viewMode==2 ? qsTr("Feeds") : root.title
        }

        delegate: ListDelegate {
            id: listItem

            anchors { left: parent.left; right: parent.right }
            iconSize: Theme.iconSizeSmall
            iconVisible: settings.showTabIcons
            titleText: model.title
            unread: model.unread
            showUnread: true
            titleColor: model.unread>0 ? Theme.primaryColor : Theme.secondaryColor

            FreshDash {
                visible: model.fresh>0
            }

            onClicked: {
                utils.setEntryModel(model.uid);
                pageStack.push(Qt.resolvedUrl("EntryPage.qml"),{"title": model.title, "index": model.index});
            }

            onHolded: contextMenu.openMenu(model.index, model.read, model.unread)

            Connections {
                target: settings
                onShowTabIconsChanged: {
                    if (model.icon!=="")
                        iconSource = cache.getUrlbyUrl(model.icon);
                    else
                        iconSource = "";
                }
            }

            Component.onCompleted: {
                if (model.icon!=="") {
                    iconSource = cache.getUrlbyUrl(model.icon);
                } else {
                    iconSource = "";
                }
            }

        }

        ViewPlaceholder {
            enabled: listView.count == 0
            text: qsTr("No feeds")
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

        onStatusChanged: {
            if (progressPanelDm.open) {
                if (status===DialogStatus.Opening) {
                    progressPanelDm.visible = false;
                }
                if (status===DialogStatus.Closed) {
                    progressPanelDm.visible = true;
                }
            }
        }

        MenuLayout {
            MenuItem {
                text: qsTr("Mark all as read")
                enabled: contextMenu.unread!=0
                //visible: enabled
                onClicked: {
                    feedModel.markAsRead(contextMenu.index);
                }
            }
            MenuItem {
                text: qsTr("Mark all as unread")
                enabled: contextMenu.read!=0
                //visible: enabled
                onClicked: {
                    feedModel.markAsUnread(contextMenu.index);
                }
            }
        }
    }

    ScrollDecorator { flickableItem: listView }
}
