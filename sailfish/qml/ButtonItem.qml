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

Item {
    id: root

    property alias button: _button
    property alias description: desc.text

    opacity: enabled ? 1.0 : 0.4
    height: col.height + 5*Theme.paddingSmall

    anchors {left: parent.left; right: parent.right}

    Column {
        id: col

        spacing: Theme.paddingSmall

        anchors {
            top: parent.top; topMargin: 3*Theme.paddingSmall
            left: parent.left; right: parent.right
            leftMargin: Theme.horizontalPageMargin; rightMargin: Theme.horizontalPageMargin
        }

        Button {
            id: _button
            enabled: root.enabled
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Label {
            id: desc
            anchors { left: parent.left; right: parent.right }
            wrapMode: Text.Wrap
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.secondaryColor
        }
    }
}
