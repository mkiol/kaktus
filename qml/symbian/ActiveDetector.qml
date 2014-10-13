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
    property int oldStatus: PageStatus.Active
    signal activated()
    signal init()

    Connections {
        target: parent

        onStatusChanged: {
            if (oldStatus === PageStatus.Inactive
                    && parent.status  === PageStatus.Active) {
                //if (!parent.showBar)
                bar.hide();
                //selector.open=false;
                activated();

            }
            if (parent.status === PageStatus.Inactive)
                oldStatus = parent.status ;
        }
    }

    Component.onCompleted: {
        //if (!parent.showBar)
        bar.hide();
        //selector.open=false;
        init();
    }
}

