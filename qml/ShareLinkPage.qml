/*
  Copyright (C) 2017 Michal Kosciesza <michal@mkiol.net>

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
//import Sailfish.TransferEngine 1.0 // not available in harbour package

Page {
    id: root

    property string link
    property string linkTitle

    readonly property bool showBar: false

    ShareMethodList {
        id: shareMethodList

        anchors { top: parent.top; left: parent.left; right: parent.right }
        height: app.flickHeight
        Behavior on height {NumberAnimation { duration: 200; easing.type: Easing.OutQuad }}
        clip: true

        header: PageHeader {
            title: qsTr("Share link")
        }

        filter: "text/x-url"
        content: {
            "type": "text/x-url",
            "status": root.link,
            "linkTitle": root.linkTitle
        }

        ViewPlaceholder {
            enabled: shareMethodList.model.count === 0
            text: qsTr("No sharing accounts available. You can add accounts in settings")
        }
    }
}
