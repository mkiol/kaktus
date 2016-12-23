/*
  Copyright (C) 2016 Michal Kosciesza <michal@mkiol.net>

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

    property alias leftIconSource: leftIcon.source
    property alias rightIconSource: rightIcon.source
    property alias minimumValue: slider.minimumValue
    property alias maximumValue: slider.maximumValue
    property alias value: slider.value
    property alias label: slider.label
    property alias valueText: slider.valueText
    property alias stepSize: slider.stepSize

    signal clicked

    anchors { left: parent.left; right: parent.right}
    height: Theme.itemSizeSmall + 3 * Theme.paddingLarge

    Image {
        id: leftIcon
        width: Theme.itemSizeSmall
        height: Theme.itemSizeSmall
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
    }

    Slider {
        id: slider
        width: parent.width - 2*Theme.itemSizeSmall
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        onClicked: root.clicked()
        leftMargin: 0
        rightMargin: 0
    }

    Image {
        id: rightIcon
        width: Theme.itemSizeSmall
        height: Theme.itemSizeSmall
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
    }

}
