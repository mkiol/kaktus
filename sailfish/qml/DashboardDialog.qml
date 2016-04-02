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

Dialog {
    id: root

    canAccept: false
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

        spacing: Theme.paddingMedium
        model: dashboardModel

        header: DialogHeader {
            title: qsTr("Dashboards")
            acceptText : qsTr("Change")
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
                    wrapMode: Text.AlignLeft
                    anchors.left: parent.left; anchors.right: parent.right;
                    anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
                    font.pixelSize: Theme.fontSizeMedium
                    text: title
                    color: listItem.down ? Theme.highlightColor : Theme.primaryColor
                }
            }

            onClicked: {
                settings.dashboardInUse = uid;
                root.canAccept = true;
                root.accept();
            }

        }

        ViewPlaceholder {
            id: placeholder
            enabled: listView.count == 0
            text: qsTr("No dashboards")
        }

        VerticalScrollDecorator {
            flickable: listView
        }

    }

    onAccepted: {
    }
}
