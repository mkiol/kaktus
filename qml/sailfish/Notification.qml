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

    rotation: app.orientation==Orientation.Portrait ? 0 : 90
    transformOrigin: Item.TopLeft
    height: app.orientation==Orientation.Portrait ? label.height + 2*Theme.paddingSmall : label.height + 1*Theme.paddingSmall
    width: app.orientation==Orientation.Portrait ? app.width : app.height
    y: app.orientation==Orientation.Portrait ? 0 : 0
    x: app.orientation==Orientation.Portrait ? 0 : app.width

    color: Theme.highlightBackgroundColor
    opacity: timer.running ? 1.0 : 0.0

    MouseArea {
        anchors.fill: parent
        onClicked: timer.stop()
    }

    Behavior on opacity { FadeAnimation {} }

    function show(text) {
        label.text = text;
        timer.restart();
    }

    Label {
        id: label
        font.pixelSize: Theme.fontSizeSmall
        font.family: Theme.fontFamily
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left; anchors.leftMargin: Theme.paddingMedium
        anchors.right: parent.right; anchors.rightMargin: Theme.paddingMedium
        opacity: 0.8
        wrapMode: Text.WordWrap
        color: Theme.highlightDimmerColor
    }

    Timer {
        id: timer
        interval: 4000
    }
}
