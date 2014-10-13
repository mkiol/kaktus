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
import com.nokia.meego 1.0

import "Theme.js" as Theme

Item {
    id: root

    property alias text: label.text
    property alias value: value.text

    signal clicked()

    anchors {
        left: parent.left; leftMargin: UiConstants.DefaultMargin
        right: parent.right; rightMargin: UiConstants.DefaultMargin
    }

    height: Math.max(label.height,value.height)

    Label {
        id: label
        horizontalAlignment: Text.AlignLeft
        anchors.verticalCenter: parent.verticalCenter
        anchors {left: parent.left; right: value.left; rightMargin: UiConstants.DefaultMargin}
    }

    Button {
        id: value

        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        visible: text!=""
        onClicked: root.clicked()
        width: text!="" ? parent.width/2 : 0

        ToolIcon {
            id: filterImage
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            //platformIconId: "textinput-combobox-arrow" icon-m-common-combobox-arrow
            platformIconId: "common-combobox-arrow"
        }
    }
}
