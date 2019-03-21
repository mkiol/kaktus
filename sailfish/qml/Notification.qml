/*
  Copyright (C) 2016-2019 Michal Kosciesza <michal@mkiol.net>

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
import Nemo.Notifications 1.0

Notification {
    id: root

    expireTimeout: 4000
    maxContentLines: 10

    function show(bodyText, summaryText, clickedHandler) {
        //console.log("show: " + bodyText + " " + summaryText)
        if (!bodyText || bodyText.length === 0)
            return

        if (bodyText === root.body)
            close()

        if (clickedHandler)
            root.connect.clicked = clickedHandler
        summaryText = summaryText ? summaryText : ""
        replacesId = 0
        body = bodyText
        previewBody = bodyText
        summary = summaryText
        previewSummary = summaryText
        publish()
    }
}
