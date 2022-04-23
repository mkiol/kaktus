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

Row {
    id: root

    property alias title: label1.text
    property alias description: label2.text

    anchors.left: parent.left; anchors.right: parent.right
    anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
    spacing: Theme.paddingLarge

    Column {
        spacing: Theme.paddingSmall
        anchors.top: parent.top
        width: parent.width-1*Theme.paddingLarge

        Label {
            id: label1
            width: parent.width
            wrapMode: Text.WordWrap
            color: Theme.highlightColor
        }

        Label {
            id: label2
            width: parent.width
            wrapMode: Text.WordWrap
            font.pixelSize: Theme.fontSizeSmall
            color: Theme.highlightColor
            linkColor: Theme.primaryColor
            textFormat: Text.StyledText
            onLinkActivated: {
                Qt.openUrlExternally(link);
            }
        }

        Spacer {}
    }
}
