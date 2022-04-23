/*
  Copyright (C) 2014-2019 Michal Kosciesza <michal@mkiol.net>

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

        spacing: Theme.paddingMedium
        model: dashboardModel

        header: DialogHeader {
            title: qsTr("Dashboards")
            acceptText : qsTr("Change")
        }

        delegate: SimpleListItem {
            title: model.title
            showPlaceholder: true

            onClicked: {
                settings.dashboardInUse = uid;
                root.canAccept = true;
                root.accept();
            }
        }

        ViewPlaceholder {
            id: placeholder
            enabled: listView.count === 0
            text: qsTr("No dashboards")
        }

        VerticalScrollDecorator {
            flickable: listView
        }
    }
}
