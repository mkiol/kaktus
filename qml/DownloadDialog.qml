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


Dialog {
    id: root

    Column {
        anchors {
            left: parent.left; leftMargin: Theme.paddingMedium
            right: parent.right; rightMargin: Theme.paddingMedium
        }
        spacing: Theme.paddingSmall

        DialogHeader {
            title: qsTr("Offline caching")
            acceptText : qsTr("Start")
        }

        Label {
            anchors {
                left: parent.left; leftMargin: Theme.paddingMedium
                right: parent.right; rightMargin: Theme.paddingMedium
            }
            wrapMode: Text.WordWrap
            text: "There are "+dm.itemsToDownloadCount()+" articles to download.\nDo you want to start?"
        }
    }

    onAccepted: {
        if (!dm.isBusy())
            dm.startFeedDownload();
    }

}
