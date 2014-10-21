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

PageStackWindow {
    id: app

    initialPage: Page {
        tools: commonTools

        Label {
            anchors.centerIn: parent
            text: "Test!"
        }
    }

    Menu {
        id: menu
        visualParent: pageStack
        MenuLayout {
            MenuItem {
                text: qsTr("Exit")
                onClicked: Qt.quit()
            }
        }
    }

    ToolBarLayout {
        id: commonTools

        ToolButton {
            iconSource: pageStack.depth > 1 ? "toolbar-back" : "close.png"
            onClicked: {
                if(pageStack.depth>1) {
                    pageStack.pop();
                } else {
                    Qt.quit()
                }
            }
        }

        ToolButton {
            iconSource: "toolbar-refresh"
        }

        ToolButton {
            iconSource: "offline.png"
        }

        ToolButton {
            iconSource: "toolbar-view-menu"
            onClicked: (menu.status == DialogStatus.Closed) ? menu.open() : menu.close()
        }

    }

}
