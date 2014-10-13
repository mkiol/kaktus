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

ToolBarLayout {
    id: commonTools
    property Item menu

    function clean() {
        menu.close();
    }

    ToolIcon {
        iconId: pageStack.depth > 1 ? "toolbar-back" : "toolbar-close"
        onClicked: {
            clean();
            bar.hide();
            if(pageStack.depth>1) {
                pageStack.pop();
            } else {
                Qt.quit()
            }
        }
    }

    ToolIcon {
        iconId: "toolbar-refresh"
        enabled: !fetcher.busy && !dm.busy
        onClicked: {
            clean();
            bar.hide();selector.open=false;
            fetcher.update();
        }
    }

    ToolIcon {
        iconSource: settings.viewMode==0 ? "vm0.png" :
                    settings.viewMode==1 ? "vm1.png" :
                    settings.viewMode==3 ? "vm3.png" :
                    settings.viewMode==4 ? "vm4.png" :
                    settings.viewMode==5 ? "vm5.png" :
                    "vm0.png"
        onClicked: {
            clean();
            if (bar.open)
                bar.hide();
            else
                bar.show();
        }
    }

    ToolIcon {
        iconSource: settings.offlineMode ?
                        (theme.inverted ? "offline-inverted.png" : "offline.png") :
                        (theme.inverted ? "online-inverted.png" : "online.png")
        onClicked: {
            clean();
            bar.hide();
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

    ToolIcon {
        iconId: "toolbar-view-menu"
        onClicked: {
            bar.hide();
            if (menu.status === DialogStatus.Closed)
                menu.open();
            else
                menu.close();
        }
    }

}
