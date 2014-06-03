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

Rectangle {
    id: root

    property bool isPortrait: screen.currentOrientation==Screen.Portrait || screen.currentOrientation==Screen.PortraitInverted

    anchors.left: parent.left; anchors.right: parent.right
    //anchors.bottom: parent.bottom
    anchors.top: parent.top; anchors.topMargin: Theme.statusBarHeight
    //height: label.height + 2*UiConstants.DefaultMargin

    height: isPortrait ? UiConstants.HeaderDefaultHeightPortrait: UiConstants.HeaderDefaultHeightLandscape

    color: "black"
    opacity: timer.running ? 0.8 : 0.0

    MouseArea {
        anchors.fill: parent
        onClicked: timer.stop()
    }

    Behavior on opacity { NumberAnimation { duration: 300 } }

    function show(text) {
        label.text = text;
        timer.restart();
    }

    Label {
        id: label
        font.pixelSize: 22
        font.family: UiConstants.SmallTitleFont
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left; anchors.leftMargin: UiConstants.DefaultMargin
        anchors.right: parent.right; anchors.rightMargin: UiConstants.DefaultMargin
        opacity: 0.8
        wrapMode: Text.WordWrap
        color: Theme.highlightForegroundColor
    }

    Timer {
        id: timer
        interval: 4000
    }

}
