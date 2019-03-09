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

    property bool showPlaceholder: false
    property alias text: _placeholder.text
    property alias icon: _icon.orgSource
    property bool small: false

    height: root.small ? Theme.iconSizeMedium * 0.7 : Theme.iconSizeMedium
    width: height

    IconPlaceholder {
        // placeholder
        id: _placeholder
        anchors.fill: parent
        visible: root.showPlaceholder &&
                 _icon.status !== Image.Ready &&
                 title.length > 0
    }

    CachedImage {
        // feed icon
        id: _icon
        anchors.fill: parent
    }
}
