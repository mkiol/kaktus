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
import com.nokia.symbian 1.1

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

    ListView {
        id: listView

        model: dashboardModel

        anchors.fill: parent
        anchors.margins: platformStyle.paddingMedium

        clip: true

        header: PageHeader {
            id: header
            title: qsTr("Select Dashboard")
        }

        delegate: ListDelegate {
            id: listItem

            anchors { left: parent.left; right: parent.right }

            showUnread: false
            titleText: model.title

            onClicked: {
                settings.dashboardInUse = uid;
                pageStack.pop();
            }
        }

        ViewPlaceholder {
            enabled: listView.count == 0
            text: qsTr("No dashboards")
        }
    }

    ScrollDecorator { flickableItem: listView }
}
