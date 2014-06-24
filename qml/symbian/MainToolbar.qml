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
        enabled: !fetcher.busy && !dm.busy
        onClicked: fetcher.update()
    }

    ToolButton {
        iconSource: settings.offlineMode ? "offline.png" : "online.png"
        onClicked: {
            if (settings.offlineMode) {
                if (dm.online)
                    settings.offlineMode = false;
                else
                    notification.show(qsTr("Cannot switch to Online mode\nNetwork connection is unavailable"));
            } else {
                settings.offlineMode = true;
            }
        }
    }

    ToolButton {
        iconSource: "toolbar-view-menu"
        onClicked: (menu.status == DialogStatus.Closed) ? menu.open() : menu.close()
    }

}
