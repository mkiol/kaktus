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
    property bool active: opacity>0
    opacity: -parent.contentY/height
    y: parent.contentY+height>0 ? -(parent.contentY + height): 0
    height: content.height + 2*UiConstants.DefaultMargin
    anchors {left: parent.left; right: parent.right}
    Column {
        id: content
        anchors {verticalCenter: parent.verticalCenter; horizontalCenter: parent.horizontalCenter}
        Label {
            id: label
            color: Theme.secondaryColor
        }
    }

    onActiveChanged: {
        if (active) {
            var lastSync = settings.lastUpdateDate;
            if (lastSync>0)
                label.text = qsTr("Last sync: %1").arg(utils.getHumanFriendlyTimeString(lastSync));
            else
                label.text = qsTr("Not yet synced");
        }
    }
}
