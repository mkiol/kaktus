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

Item {
    id: root

    property alias text: textswitch.text
    property alias description: textswitch.description
    property alias checked: textswitch.checked
    property alias iconSource: icon.source

    anchors.left: parent.left; anchors.right: parent.right
    height: Math.max(textswitch.height, icon.height)

    TextSwitch {
        id: textswitch
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width - icon.width - icon.anchors.rightMargin
    }

    Image {
        id: icon
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: Theme.paddingMedium
        source: "reader.png"
    }
}
