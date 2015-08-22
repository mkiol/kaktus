/*
  Copyright (C) 2015 Michal Kosciesza <michal@mkiol.net>

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

    property bool showBar: false

    allowedOrientations: {
        switch (settings.allowedOrientations) {
        case 1:
            return Orientation.Portrait;
        case 2:
            return Orientation.Landscape;
        }
        return Orientation.Landscape | Orientation.Portrait;
    }

    ActiveDetector {}

    SilicaListView {
        id: listView

        anchors { top: parent.top; left: parent.left; right: parent.right }
        clip: true

        height: app.flickHeight

        model:  ListModel {
            ListElement { name: "Netvibes"; iconSource: "nv.png"; type: 1}
            ListElement { name: "Old Reader"; iconSource: "oldreader.png"; type: 2}
            ListElement { name: "Feedly (comming soon)"; iconSource: "feedly.png"; type: 3}
        }

        header: PageHeader {
            title: qsTr("Add account")
        }

        delegate: ListItem {
            id: listItem
            contentHeight: item.height + 2 * Theme.paddingMedium
            highlighted: root.accountType == type
            enabled: type != 3
            opacity: enabled ? 1.0 : 0.5

            Column {
                id: item
                spacing: Theme.paddingSmall
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width

                Item {
                    anchors.left: parent.left; anchors.right: parent.right;
                    height: Math.max(icon.height,label.height)

                    Image {
                        id: icon
                        anchors { left: parent.left }
                        source: iconSource
                    }

                    Label {
                        id: label
                        wrapMode: Text.AlignLeft
                        anchors.left: icon.right; anchors.right: parent.right;
                        anchors.leftMargin: Theme.paddingMedium; anchors.rightMargin: Theme.paddingLarge
                        font.pixelSize: Theme.fontSizeMedium
                        text: name
                        color: listItem.down ? Theme.highlightColor : Theme.primaryColor
                    }
                }
            }

            onClicked: {
                if (type == 1) {
                    app.reconnectFetcher(1);
                    pageStack.replaceAbove(pageStack.previousPage(),Qt.resolvedUrl("NvSignInDialog.qml"),{"code": 400});
                }
                if (type == 2) {
                    app.reconnectFetcher(2);
                    pageStack.replaceAbove(pageStack.previousPage(),Qt.resolvedUrl("OldReaderSignInDialog.qml"),{"code": 400});
                }
                if (type == 3) {
                    app.reconnectFetcher(3);
                    utils.resetQtWebKit();
                    fetcher.getConnectUrl(20);
                    //pageStack.replaceAbove(pageStack.previousPage(),Qt.resolvedUrl("FeedlySignInDialog.qml"),{"code": 400});
                }
            }

        }

        VerticalScrollDecorator {
            flickable: listView
        }

    }
}
