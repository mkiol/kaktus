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



Rectangle {
    id: root

    height: visible ? Theme.itemSizeMedium * 0.7 : 0
    width: parent.width

    anchors.bottom: parent.bottom
    anchors.left: parent.left

    //color: Theme.highlightBackgroundColor
    color: Theme.rgba(Theme.highlightBackgroundColor, 0.0)
    enabled: opacity > 0.0

    opacity: 1.0
    visible: opacity > 0.0
    Behavior on opacity { FadeAnimation {} }

    IconButton {
        id: offlineButton

        //height: 48
        //width: 48
        //smooth: true

        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        //anchors.rightMargin: Theme.paddingSmall

        icon.source: offLineMode ? "image://theme/icon-status-wlan-no-signal" : "image://theme/icon-status-wlan-4"
        onClicked: offLineMode = !offLineMode;
    }
}
