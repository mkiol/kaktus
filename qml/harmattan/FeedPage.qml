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
        title: root.title
    }*/

    ListView {
        id: listView

        model: feedModel

        //anchors { top: header.bottom; left: parent.left; right: parent.right; bottom: parent.bottom }
        anchors.fill: parent

        clip: true

        PullBar {}

        delegate: ListDelegate {
            id: listItem

            anchors { left: parent.left; right: parent.right }

            iconSize: Theme.iconSizeSmall
            iconVisible: settings.showTabIcons
            titleText: model.title
            unread: model.unread
            showUnread: true
            titleColor: model.unread>0 ? Theme.primaryColor : Theme.secondaryColor

            onClicked: {
                utils.setEntryModel(uid);
                pageStack.push(Qt.resolvedUrl("EntryPage.qml"),{"title": title, "index": model.index});
            }

            onHolded: contextMenu.open()

            Connections {
                target: settings
                onShowTabIconsChanged: {
                    if (settings.showTabIcons && model.icon!=="")
                        iconSource = cache.getUrlbyUrl(model.icon);
                    else
                        iconSource = "";
                }
            }

            Component.onCompleted: {
                if (settings.showTabIcons && model.icon!=="") {
                    iconSource = cache.getUrlbyUrl(model.icon);
                } else {
                    iconSource = "";
                }
            }

            Dialog {
                id: contextMenu
                buttons: Column {
                    spacing: UiConstants.DefaultMargin

                    Button {
                        text: qsTr("Mark all as read")
                        enabled: model.unread!=0
                        visible: enabled
                        onClicked: {
                            feedModel.markAllAsRead(model.index);
                            tabModel.updateFlags();
                            contextMenu.accept();
                        }
                    }

                    Button {
                        text: qsTr("Mark all as unread")
                        enabled: model.read!=0
                        visible: enabled
                        onClicked: {
                            feedModel.markAllAsUnread(model.index);
                            tabModel.updateFlags();
                            contextMenu.accept();
                        }
                    }
                }
            }
        }

        ViewPlaceholder {
            enabled: listView.count == 0
            text: qsTr("No feeds")
        }
    }

    ScrollDecorator { flickableItem: listView }
}
