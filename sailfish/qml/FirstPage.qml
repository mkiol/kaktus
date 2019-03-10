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

    SilicaListView {
        id: listView

        anchors { top: parent.top; left: parent.left; right: parent.right }
        height: app.flickHeight
        Behavior on height {NumberAnimation { duration: 200; easing.type: Easing.OutQuad }}
        clip: true

        PullDownMenu {
            id: menu
            //enabled: !guide.open

            MenuItem {
                text: qsTr("About")
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("AboutPage.qml"));
                }
            }

            MenuItem {
                text: qsTr("Add account")
                onClicked: pageStack.push(Qt.resolvedUrl("AccountsDialog.qml"));
                enabled: !settings.signedIn && !app.fetcherBusyStatus && !dm.busy && !dm.removerBusy
            }

            MenuItem {
                text: enabled ? qsTr("Sync") : qsTr("Busy...")
                visible: settings.signedIn && enabled
                onClicked: fetcher.update()
                enabled: !fetcher.busy && !dm.busy && !dm.removerBusy
            }
        }

        ViewPlaceholder {
            id: placeholder
            enabled: true
            text: settings.signedIn ?
                      app.fetcherBusyStatus || dm.busy ? qsTr("Wait until sync finish") :
                      qsTr("To do feeds synchronization, pull down and select sync.") :
                      qsTr("You are not signed in to any account. Pull down to add one.")
        }

        VerticalScrollDecorator {
            flickable: listView
        }
    }
}
