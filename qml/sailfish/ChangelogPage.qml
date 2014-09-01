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
            title: qsTr("Changelog")
        }

        model: VisualItemModel {

            SectionHeader {
                text: qsTr("1.2.0")
            }

            Label {
                wrapMode: Text.WordWrap
                font.pixelSize: Theme.fontSizeExtraSmall
                anchors.left: parent.left; anchors.right: parent.right
                anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
                text: '* Double-click marks article as read/unread'
            }

            Label {
                wrapMode: Text.WordWrap
                font.pixelSize: Theme.fontSizeExtraSmall
                anchors.left: parent.left; anchors.right: parent.right
                anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
                text: '* Option to show all feeds in one list (see "Browsing Mode option")'
            }

            Label {
                wrapMode: Text.WordWrap
                font.pixelSize: Theme.fontSizeExtraSmall
                anchors.left: parent.left; anchors.right: parent.right
                anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
                text: '* Indicator for articles that have been added since last sync'
            }

            Label {
                wrapMode: Text.WordWrap
                font.pixelSize: Theme.fontSizeExtraSmall
                anchors.left: parent.left; anchors.right: parent.right
                anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
                text: '* Option to delete cache data (see "Cache size" option)'
            }

            Label {
                wrapMode: Text.WordWrap
                font.pixelSize: Theme.fontSizeExtraSmall
                anchors.left: parent.left; anchors.right: parent.right
                anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
                text: '* Many UI improvements'
            }
        }

        VerticalScrollDecorator {}
    }

}
