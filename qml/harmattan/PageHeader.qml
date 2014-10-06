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

    property alias title: label.text
    property bool isPortrait: screen.currentOrientation==Screen.Portrait || screen.currentOrientation==Screen.PortraitInverted

    //z: 100
    //anchors { top: parent.top; left: parent.left; right: parent.right }
    anchors { left: parent.left; right: parent.right }
    height: isPortrait ? UiConstants.HeaderDefaultHeightPortrait: UiConstants.HeaderDefaultHeightLandscape

    Label {
        id: label

        anchors {
            verticalCenter: parent.verticalCenter;
            left: parent.left; leftMargin: UiConstants.DefaultMargin
            right: parent.right; rightMargin: UiConstants.DefaultMargin
        }

        color: Theme.primaryColor
        font: UiConstants.HeaderFont
        elide: Text.ElideRight
        horizontalAlignment: Text.AlignRight
    }
}
