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

Column {
    id: root

    property alias text: label.text
    property alias checked: sw.checked
    property alias description: desc.text

    spacing: platformStyle.paddingMedium

    anchors {
        left: parent.left
        right: parent.right
    }

    Item {
        anchors { left: parent.left; right: parent.right }
        height: Math.max(label.height,sw.height)

        Label {
            id: label
            wrapMode: Text.WordWrap
            anchors { left: parent.left; right: sw.left; verticalCenter: parent.verticalCenter}
        }

        Switch {
            id: sw
            anchors { right: parent.right; verticalCenter: parent.verticalCenter }
        }
    }

    Label {
        id: desc
        anchors { left: parent.left; right: parent.right }
        color: platformStyle.colorNormalMid
        wrapMode: Text.WordWrap
        visible: text!=""
    }
}
