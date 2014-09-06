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

            LogItem {
                title: qsTr('Multi-Feed widget support')
                description: qsTr('Kaktus can also read Feeds, which are aggregated in the Netvibes Multi-Feed widget.')
            }

            LogItem {
                title: qsTr('Double-click marks article as read/unread')
                description: qsTr('In addition to the context menu option, marking as read/unread can be done by double-click.')
            }

            LogItem {
                title: qsTr('New Browsing Modes')
                description: qsTr('There are new Browsing Modes. '+
                                  'It is possible to show all articles in the one list '+
                                  'or group by Tabs or Feeds.')
            }

            LogItem {
                title: qsTr('Indicator for new articles')
                description: qsTr('Articles, that have been added since last sync, '+
                                  'are marked with small dash on the right side of the list.')
            }

            LogItem {
                title: qsTr('Option to delete cache data')
                description: qsTr('Cache data can be deleted manually. The option is located on the settings page.')
            }

            LogItem {
                title: qsTr('Image caching improvements')
                description: qsTr('Caching mechanism for images are improved. Files with images are downloaded more effectively.')
            }

            Item {
                height: Theme.paddingMedium
            }

        }

        VerticalScrollDecorator {}
    }

}
