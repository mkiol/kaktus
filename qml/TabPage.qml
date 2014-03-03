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

    SilicaListView {
        id: listView
        anchors.fill: parent
        model: tabModel

        MainMenu{}

        header: PageHeader {
            title: "Tabs"
        }

        delegate: ListItem {
            id: listItem
            contentHeight: item.height + 2 * Theme.paddingMedium

            Column {
                id: item
                spacing: Theme.paddingSmall
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width

                Label {
                    id: label
                    wrapMode: Text.AlignLeft
                    anchors.left: parent.left; anchors.right: parent.right;
                    anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
                    font.pixelSize: Theme.fontSizeMedium
                    text: title
                }
            }

            Image {
                id: image
                width: Theme.iconSizeSmall
                height: Theme.iconSizeSmall
                anchors.right: parent.right; anchors.rightMargin: Theme.paddingLarge
                anchors.verticalCenter: item.verticalCenter
            }

            Component.onCompleted: image.source = cache.getUrlbyUrl(iconUrl)

            onClicked: {
                utils.setFeedModel(uid);
                pageStack.push(Qt.resolvedUrl("FeedPage.qml"),{"title": title});
                console.log(cache.getUrlbyUrl(iconUrl));
            }
        }


        ViewPlaceholder {
            enabled: listView.count == 0
            text: qsTr("No tabs")

            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.secondaryHighlightColor
                //text: signedIn ? qsTr("Try to Sync!") : qsTr("You are not signed in to Netvibes!")
                text: qsTr("Try to Sync!")
            }
        }

        /*Image {
            visible: listView.count == 0
            anchors.centerIn: parent
            source: "image://theme/graphic-tutorial-close?"+Theme.secondaryHighlightColor
        }*/

        /*Button {
            visible: !signedIn
            enabled: !signedIn
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: Theme.itemSizeLarge
            text: qsTr("Sign In")
        }*/

        VerticalScrollDecorator {
            flickable: listView
        }

    }

    ControlBarDark {
        id: controlbar
        canSync: false
        canOffline: true

        onSyncClicked: {
            fetcher.update();
        }
    }

}
