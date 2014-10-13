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

    property bool open: false

    height: inPortrait ? privateStyle.toolBarHeightPortrait : privateStyle.toolBarHeightLandscape

    enabled: open
    opacity: open ? 1.0 : 0.0
    visible: opacity > 0.0
    Behavior on opacity { NumberAnimation { duration: 300 } }

    Rectangle {
        id: background
        anchors.fill: parent
        color: Qt.rgba(0.1,0.1,0.1, 0.9)
    }

    function show() {
        root.open = true;
    }

    function hide() {
        root.open = false;
    }

    MouseArea {
        anchors.fill: parent
        onClicked: bar.hide();
    }

    Row {
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: spacing
        spacing: (parent.width-5*icon.width)/6

        ToolButton {
            id: icon
            iconSource: "vm0.png"

            onClicked: {
                bar.hide();
                settings.viewMode = 0;
            }
        }

        ToolButton {
            iconSource: "vm1.png"

            onClicked: {
                bar.hide();
                settings.viewMode = 1;
            }
        }

        ToolButton {
            iconSource: "vm3.png"

            onClicked: {
                bar.hide();
                settings.viewMode = 3;
            }
        }

        ToolButton {
            iconSource: "vm4.png"

            onClicked: {
                bar.hide();
                settings.viewMode = 4;
            }
        }

        ToolButton {
            iconSource: "vm5.png"

            onClicked: {
                bar.hide();
                settings.viewMode = 5;
            }
        }
    }

}
