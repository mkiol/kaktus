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

import "Theme.js" as Theme

Image {
    id: root
    property bool open: false

    opacity: open ? 1.0 : 0.0
    visible: opacity > 0.0
    Behavior on opacity { NumberAnimation {duration: 300} }

    source: "selector.png"

    Behavior on x { NumberAnimation { duration: 200;easing.type: Easing.OutQuad } }

    PropertyAnimation on opacity {
        loops: Animation.Infinite
        from: 0.0
        to: 1.0
        duration: 1200
        running: root.open && Qt.application.active
    }
}
