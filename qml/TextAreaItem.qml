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

    property alias textArea: _textArea
    property alias description: desc.text

    opacity: enabled ? 1.0 : 0.4
    height: col.height + 4*Theme.paddingSmall

    anchors {left: parent.left; right: parent.right}

    Column {
        id: col
        anchors {
            top: parent.top; topMargin: 2*Theme.paddingSmall
            left: parent.left; right: parent.right
        }

        TextArea {
            id: _textArea
            enabled: root.enabled
            anchors {left: parent.left; right: parent.right}
        }

        Label {
            id: desc
            anchors {
                left: parent.left; right: parent.right;
                leftMargin: Theme.horizontalPageMargin; rightMargin: Theme.horizontalPageMargin
            }
            wrapMode: Text.Wrap
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.secondaryColor
        }
    }
}
