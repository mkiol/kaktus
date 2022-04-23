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

Rectangle {
    property int count

    width: unreadlabel.width + 3 * Theme.paddingSmall
    height: unreadlabel.height + 2 * Theme.paddingSmall
    color: Theme.rgba(Theme.highlightBackgroundColor, 0.2)
    radius: 5

    Label {
        id: unreadlabel
        text: count
        anchors.centerIn: parent
        color: Theme.colorScheme ? Qt.darker(Theme.highlightColor) : Theme.highlightColor
    }
}
