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



PushUpMenu {

    /*MenuLabel {
        text: app.offLineMode ? qsTr("Offline") : qsTr("Online")
    }

    MenuItem {
        //text: app.offLineMode ? qsTr("Switch to Online") : qsTr("Switch to Offline")
        text: qsTr("Switch mode")
        onClicked: {
            if (app.offLineMode)
                settings.setOfflineMode(false);
            else
                settings.setOfflineMode(true);
            offLineMode = settings.getOfflineMode();
        }
    }*/

    Row {
        Switch{
            id: pause
            iconSource: "image://theme/icon-m-wlan"
            onClicked: offLineMode = !offLineMode;
            enabled: !offLineMode
        }

    }

}
