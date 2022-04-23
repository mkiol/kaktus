/*
  Copyright (C) 2017 Michal Kosciesza <michal@mkiol.net>

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

Image {
    id: root

    property int maxWidth: 0
    property int minWidth: 0
    property string orgSource: ""
    property bool cached: true
    property bool hidden: false
    readonly property bool ok: status === Image.Ready &&
                               (minWidth === 0 || (sourceSize.width > minWidth &&
                                                   sourceSize.height > minWidth))
    readonly property bool filled: maxWidth > 0 && implicitWidth > maxWidth/3

    width: filled ? maxWidth : sourceSize.width
    enabled: !hidden && ok
    visible: opacity > 0 && enabled
    opacity: enabled ? 1.0 : 0.0
    Behavior on opacity {
        NumberAnimation { duration: 200 }
    }

    onOrgSourceChanged: {
        if (orgSource.length > 0) {
            if (cached) {
                var cachedSource = cserver.getPathByUrl(orgSource)
                if (cachedSource.length === 0) {
                    if (!settings.offlineMode && dm.online) {
                        cached = false
                        source = orgSource
                    }
                } else {
                    source = cachedSource
                }
            } else {
                source = orgSource
            }
        } else {
            source = ""
        }
    }

    onStatusChanged: {
        if (!hidden && status === Image.Ready) {
            if (filled) {
                var ratio = width / implicitWidth
                height = implicitHeight * ratio
                if (width < sourceSize.width)
                    sourceSize.width = width
                if (height < sourceSize.height)
                    sourceSize.height = height
            }
        } else if (status === Image.Error &&
                   cached && orgSource.length > 0 &&
                   !settings.offlineMode && dm.online) {
            cached = false
            source = orgSource
        }
    }
}
