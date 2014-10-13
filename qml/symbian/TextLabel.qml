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
import com.nokia.symbian 1.1

Item {
    id: root

    property alias text: label.text
    property alias value: value.text

    anchors {
        left: parent.left;
        right: parent.right;
    }

    height: Math.max(label.height,value.height)

    Label {
        id: label
        anchors {left: parent.left; right: value.left; rightMargin: platformStyle.paddingMedium}
        horizontalAlignment: Text.AlignLeft
    }

    Label {
        id: value
        color: platformStyle.colorNormalMid
        anchors.right: parent.right
        visible: text!=""
    }
}
