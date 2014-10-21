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

import "Theme.js" as Theme

Page {
    id: root

    tools: SimpleToolbar {}

    orientationLock: {
        switch (settings.allowedOrientations) {
        case 1:
            return PageOrientation.LockPortrait;
        case 2:
            return PageOrientation.LockLandscape;
        }
        return PageOrientation.Automatic;
    }

    ActiveDetector {}

    ListView {
        id: listView

        anchors.fill: parent

        spacing: platformStyle.paddingLarge

        header: PageHeader {
            title: qsTr("Changelog")
        }

        model: VisualItemModel {

            SectionHeader {
                text: qsTr("Version %1").arg("1.2.1")
            }

            LogItem {
                title: 'Option to change language'
                description: 'UI language can be changed on settings page.'
            }

            LogItem {
                title: 'Farsi language'
                description: 'Persian language is supported (thanks to Ali Adineh).'
            }

            LogItem {
                title: 'Multi-Feed widget support'
                description: 'Kaktus can read RSS feeds, which are aggregated with '+
                                  'Netvibes Multi-Feed widget. So far, only simple Feed '+
                                  'widget was supported.'
            }

            LogItem {
                title: 'Double-click marks article as read/unread'
                description: 'In addition to the context menu option, marking as '+
                                  'read/unread now can be done by double-click.'
            }

            LogItem {
                title: 'New View Modes'
                description: 'There are new View Modes, which enable you to '+
                                  'show all articles in the one list or group articles '+
                                  'using tabs. You can switch between modes by clicking '+
                                  'on the appropriate icon on the bottom bar.'
            }

            LogItem {
                title: 'Slow feeds'
                description: 'One of the new view modes gives you option to '+
                                  'view only articles from less frequently updated feeds '+
                                  '- so called Slow feeds. '
            }

            LogItem {
                title: 'Indicator for new articles'
                description: 'Articles, that have been added since last sync, '+
                                  'are marked with small dash on the right side of the list.'
            }

            LogItem {
                title: 'Option to delete cache data'
                description: 'Cache data can be deleted manually. The option is located on the Settings page.'
            }

            /*LogItem {
                title: 'User Guide'
                description: 'User Guide contains information how to navigate the new UI elements like Bottom Bar and View Modes.'
            }*/

            LogItem {
                title: 'User Interface improvements'
                description: 'Few user interface improvements were added.'
            }

            Item {
                height: platformStyle.paddingMedium
            }

        }
    }

    ScrollDecorator { flickableItem: listView }
}
