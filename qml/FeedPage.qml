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

    property string title

    SilicaListView {
        id: listView
        model: feedModel

        anchors { top: parent.top; left: parent.left; right: parent.right }
        height: app.height - (dm.busy||fetcher.busy ? Theme.itemSizeMedium : 0);
        clip:true

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
                width: parent.width

                Label {
                    wrapMode: Text.AlignLeft
                    anchors.left: parent.left; anchors.right: parent.right;
                    anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
                    font.pixelSize: Theme.fontSizeMedium
                    text: title
                }

                Label {
                    anchors.left: parent.left; anchors.right: parent.right;
                    anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
                    font.pixelSize: Theme.fontSizeSmall
                    color: Theme.secondaryColor
                    text: {
                        if (unread==0)
                            return qsTr("all read");
                        if (unread==1)
                            return qsTr("1 unread");
                        if (unread<5)
                            return qsTr("%1 unread","less than 5 articles are unread").arg(unread);
                        return qsTr("%1 unread","more or equal 5 articles are unread").arg(unread);
                    }
                    visible: unread!=0
                }
            }

            Image {
                id: image
                width: Theme.iconSizeSmall
                height: Theme.iconSizeSmall
                anchors.right: parent.right; anchors.rightMargin: Theme.paddingLarge
                anchors.verticalCenter: item.verticalCenter
                visible: settings.showTabIcons
            }

            Connections {
                target: settings
                onShowTabIconsChanged: {
                    if (settings.showTabIcons)
                        image.source = cache.getUrlbyUrl(model.icon);
                    else
                        image.source = "";
                }
            }

            Component.onCompleted: {
                if (settings.showTabIcons)
                    image.source = cache.getUrlbyUrl(model.icon);
                else
                    image.source = "";
            }

            onClicked: {
                utils.setEntryModel(uid);
                pageStack.push(Qt.resolvedUrl("EntryPage.qml"),{"title": title, "index": model.index});
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
