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

Rectangle {
    anchors.top: parent.top; anchors.left: parent.left
    width: Theme.paddingSmall; height: parent.height
    visible: model.fresh>0
    radius: 10
    opacity: 0.4
    gradient: Gradient {
        GradientStop { position: 0.0; color: Theme.highlightColor }
        GradientStop { position: 1.0; color: "#00000000" }
    }
}
