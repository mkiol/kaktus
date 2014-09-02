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


Page {
    id: root

    allowedOrientations: {
        switch (settings.allowedOrientations) {
        case 1:
            return Orientation.Portrait;
        case 2:
            return Orientation.Landscape;
        }
        return Orientation.Landscape | Orientation.Portrait;
    }

    SilicaListView {
        anchors { top: parent.top; left: parent.left; right: parent.right }
        clip: true

        height: {
            if (dm.busy||fetcher.busy)
                return isPortrait ? app.height-Theme.itemSizeMedium : app.width-0.8*Theme.itemSizeMedium;
            return isPortrait ? app.height : app.width;
        }

        header: PageHeader {
            title: qsTr("What's new")
        }

        model: VisualItemModel {

            SectionHeader {
                text: "1.2.0"
            }

            LogItem {
                text: qsTr('Netvibes "Multi-Feed" widget support')
            }

            LogItem {
                text: qsTr('Double-click marks article as read/unread')
            }

            LogItem {
                text: qsTr('Option to show all feeds in one list (see "Browsing Mode option")')
            }

            LogItem {
                text: qsTr('Indicator for articles that have been added since last sync')
            }

            LogItem {
                text: qsTr('Option to delete cache data (see "Cache size" option)')
            }

            LogItem {
                text: qsTr('Improved display of images')
            }

            LogItem {
                text: qsTr('Many UI improvements')
            }

        }

        VerticalScrollDecorator {}
    }

}
