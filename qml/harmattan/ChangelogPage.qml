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

import "Theme.js" as Theme

Page {
    id: root

    tools: SimpleToolbar {}

    property bool showBar: false

    ActiveDetector {}

    orientationLock: {
        switch (settings.allowedOrientations) {
        case 1:
            return PageOrientation.LockPortrait;
        case 2:
            return PageOrientation.LockLandscape;
        }
        return PageOrientation.Automatic;
    }

    ListView {
        id: listView

        anchors.fill: parent

        spacing: Theme.paddingLarge

        header: PageHeader {
            title: qsTr("Changelog")
        }

        model: VisualItemModel {

            SectionHeader {
                text: qsTr("Version %1").arg("1.2.1")
            }

            LogItem {
                title: qsTr('Multi-Feed widget support')
                description: qsTr('Kaktus can read RSS feeds, which are aggregated with '+
                                  'Netvibes Multi-Feed widget. So far, only simple Feed '+
                                  'widget was supported.')
            }

            LogItem {
                title: qsTr('Double-click marks article as read/unread')
                description: qsTr('In addition to the context menu option, marking as '+
                                  'read/unread now can be done by double-click.')
            }

            LogItem {
                title: qsTr('New View Modes')
                description: qsTr('There are new View Modes, which enable you to '+
                                  'show all articles in the one list or group articles '+
                                  'using tabs. You can switch between modes by clicking '+
                                  'on the appropriate icon on the bottom bar.')
            }

            LogItem {
                title: qsTr('Slow feeds')
                description: qsTr('One of the new view modes gives you option to '+
                                  'view only articles from less frequently updated feeds '+
                                  '- so called Slow feeds. ')
            }

            LogItem {
                title: qsTr('Indicator for new articles')
                description: qsTr('Articles, that have been added since last sync, '+
                                  'are marked with small dash on the right side of the list.')
            }

            LogItem {
                title: qsTr('Option to delete cache data')
                description: qsTr('Cache data can be deleted manually. The option is located on the Settings page.')
            }

            LogItem {
                title: qsTr('User Guide')
                description: qsTr('User Guide contains information how to navigate the new UI elements like Bottom Bar and View Modes.')
            }

            LogItem {
                title: qsTr('User Interface improvements')
                description: qsTr('Few user interface improvements were added.')
            }

            Item {
                height: Theme.paddingMedium
            }

        }
    }

    ScrollDecorator { flickableItem: listView }
}
