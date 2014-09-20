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

Image {
    id: root

    property bool active: false

    anchors.right: parent.right; anchors.rightMargin: Theme.paddingMedium
    anchors.bottom: parent.bottom; anchors.bottomMargin: Theme.paddingMedium

    visible: opacity>0
    opacity: active ? 1.0 : 0.0
    Behavior on opacity { FadeAnimation {duration: 300} }

    source: settings.offlineMode ? "image://theme/icon-m-wlan-no-signal?"+Theme.highlightColor : "image://theme/icon-m-wlan-4?"+Theme.highlightColor
}
