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
            return Orientation.PortraitMask;
        case 2:
            return Orientation.LandscapeMask;
        }
        return Orientation.All;
    }

    ActiveDetector {}

    SilicaListView {
        id: listView

        anchors { top: parent.top; left: parent.left; right: parent.right }
        height: app.flickHeight
        Behavior on height {NumberAnimation { duration: 200; easing.type: Easing.OutQuad }}
        clip: true

        model:  ListModel {
            ListElement { name: "Netvibes"; iconSource: "image://icons/icon-m-netvibes"; type: 1}
            ListElement { name: "Old Reader"; iconSource: "image://icons/icon-m-oldreader"; type: 2}
            ListElement { name: "Tiny Tiny RSS"; iconSource: "image://icons/icon-m-ttrss"; type: 4}
        }

        header: PageHeader {
            title: qsTr("Add account")
        }

        delegate: SimpleListItem {
            highlighted: root.accountType === type
            icon: iconSource
            title: name

            onClicked: {
                if (type == 1) {
                    app.reconnectFetcher(1);
                    pageStack.replaceAbove(pageStack.previousPage(),
                                           Qt.resolvedUrl("NvSignInDialog.qml"),
                                           {"code": 400});
                } else if (type == 2) {
                    app.reconnectFetcher(2);
                    pageStack.replaceAbove(pageStack.previousPage(),
                                           Qt.resolvedUrl("OldReaderSignInDialog.qml"),
                                           {"code": 400});
                } else if (type == 4) {
                    app.reconnectFetcher(4);
                    pageStack.replaceAbove(pageStack.previousPage(),
                                           Qt.resolvedUrl("TTRssSignInDialog.qml"),
                                           {"code": 400});
                }
            }
        }

        VerticalScrollDecorator {
            flickable: listView
        }
    }
}
