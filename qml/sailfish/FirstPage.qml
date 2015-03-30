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

        clip:true

        PullDownMenu {
            id: menu

            MenuItem {
                text: qsTr("About")
                visible: root.showAbout

                onClicked: {
                    pageStack.push(Qt.resolvedUrl("AboutPage.qml"));
                }
            }

            MenuItem {
                text: qsTr("Add account")
                onClicked: pageStack.push(Qt.resolvedUrl("SignInDialog.qml"),{"code": 400});
                enabled: !settings.signedIn && !fetcher.busy && !dm.busy && !dm.removerBusy
            }
        }

        ViewPlaceholder {
            id: placeholder
            enabled: listView.count < 1
            text: settings.signedIn ?
                      fetcher.busy||dm.busy ? qsTr("Wait until Sync finish.") :
                                              qsTr("To do feeds synchronisation, pull down and select Sync.") :
                      qsTr("You are not signed in to any account. Pull down to add one.")
        }

        VerticalScrollDecorator {
            flickable: listView
        }
    }
}
