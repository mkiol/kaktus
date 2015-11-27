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

    property bool showBar: false

    allowedOrientations: {
        switch (settings.allowedOrientations) {
        case 1:
            return Orientation.Portrait;
        case 2:
            return Orientation.Landscape;
        }
        return Orientation.Landscape | Orientation.Portrait;
    }

    ActiveDetector {}

    SilicaListView {
        anchors { top: parent.top; left: parent.left; right: parent.right }
        clip: true

        height: app.flickHeight

        header: PageHeader {
            title: qsTr("Changelog")
        }

        model: VisualItemModel {

            SectionHeader {
                text: qsTr("Version %1").arg("2.2")
            }

            LogItem {
                title: 'Open articles in browser'
                description: "Option to open articles directly in default web browser instead built-in web viewer.";
            }

            SectionHeader {
                text: qsTr("Version %1").arg("2.1")
            }

            LogItem {
                title: 'Sync read articles'
                description: "Previously, unread as well as read articles were synced. With this new option comes possibility to disable syncing read articles. It will speed up synchronization, but read articles will not be accessible form Kaktus.";
            }

            LogItem {
                title: 'Sort order'
                description: 'New settings option enabling specific sort order for list of articles. Possible values: Recent first, Oldest first.'
            }

            LogItem {
                title: 'Mark above as read'
                description: 'Context menu option for marking all above articles as read.'
            }

            LogItem {
                title: 'Old Reader: Like & Liked articles view mode'
                description: "New context option to Like/Unlike article. So called \"Slow\" view mode is now replaced by Liked articles view mode.";
            }

            LogItem {
                title: 'Old Reader: Enable social features'
                description: "New option to enable/disable Old Reader's social features. If enabled, following features will be visible: Following folder, Sharing article with followers, Like/Unlike option, Liked articles view mode.";
            }

            SectionHeader {
                text: qsTr("Version %1").arg("2.0")
            }

            LogItem {
                title: 'Old Reader support'
                description: 'Old Reader is supported as new feed aggreagator.'
            }

            LogItem {
                title: 'Many small improvements and bug fixes'
                description: 'Many improvements, like performance optimization and UI polishing were made.'
            }

            SectionHeader {
                text: qsTr("Version %1").arg("1.4")
            }

            LogItem {
                title: 'UI polishing'
                description: 'UI is more in line with Sailfish design style.'
            }

            LogItem {
                title: 'Remorse popups'
                description: 'Instead dialog prompts, remorse popups are used.'
            }

            LogItem {
                title: 'New translations'
                description: 'New translations German, Spanish, Finnish, French, Italian and Chinese were added. All other translations have been updated.'
            }

            SectionHeader {
                text: qsTr("Version %1").arg("1.3")
            }

            LogItem {
                title: 'Read mode'
                description: 'When Read mode is enabled, web pages will be reformatted into an easy to read version. '+
                             'All of a website\'s native styles will be striped so you can focus on what you\'re reading. '+
                             'You can switch to Read mode using rightmost button on web viewer\'s toolbar.'
            }

            LogItem {
                title: 'Copy URL to clipboard'
                description: 'New button was added to web viewer\'s toolbar. It allows you to copy page\'s URL to clipboard.'
            }

            LogItem {
                title: 'Sign in with Twitter or Facebook'
                description: 'In addition to Netvibes credentials, sign in can be done also with Twitter or Facebook account.'
            }

            LogItem {
                title: 'Caching only on WiFi'
                description: 'Until now you could only enable or disable caching feature. '+
                             'Now, you can also set caching to start only when phone is connected with WiFi.'
            }

            SectionHeader {
                text: qsTr("Version %1").arg("1.2.3")
            }

            LogItem {
                title: 'Turkish translation'
                description: 'Turkish language is supported thanks to Mesut Aktaş.'
            }

            LogItem {
                title: 'Caching improvements'
                description: 'Caching process has been optimized and is now faster.'
            }

            LogItem {
                title: 'Show only unread option in Pull-down menu'
                description: 'Option to show only uread articles can be changed in Pull-down menu.'
            }

            LogItem {
                title: 'Support for new Tabs icons'
                description: 'Netvibes recently has changed stock Tab icons. Now, Kaktus is able to display them properly.'
            }

            SectionHeader {
                text: qsTr("Version %1").arg("1.2.2")
            }

            LogItem {
                title: 'Performance improvements'
                description: 'Some UI functions were optimized for faster performance.'
            }

            LogItem {
                title: 'Option to increase font size in web viewer'
                description: 'Web viewer font size can be changed on settings page. '+
                             'By default, "Normal" value is set and this means that the text size '+
                             'will be increased by 50% in comparison with the previous version of Kaktus. '+
                             'New option, works only on websites that have mobile version.'
            }

            LogItem {
                title: 'Russian, Czech and Dutch translations'
                description: 'Russian, Czech and Dutch languages are supported thanks to Kiratonin, fri & Stoffels.'
            }

            SectionHeader {
                text: qsTr("Version %1").arg("1.2.1")
            }

            LogItem {
                title: 'User Interface improvements'
                description: 'Few user interface improvements were added.'
            }

            LogItem {
                title: 'Option to change language'
                description: 'UI language can be changed on settings page.'
            }

            LogItem {
                title: 'Farsi translation'
                description: 'Persian language is supported (thanks to Ali Adineh).'
            }

            SectionHeader {
                text: qsTr("Version %1").arg("1.2.0")
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
                title: 'Bottom Bar & New View Modes'
                description: 'There are new View Modes, which enable you to '+
                                  'show all articles in the one list or group articles '+
                                  'using tabs. You can switch between modes by clicking '+
                                  'on the appropriate icon on the Bottom Bar.'
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

            LogItem {
                title: 'User Guide'
                description: 'User Guide contains information how to navigate the new UI elements like Bottom Bar and View Modes.'
            }

            Item {
                height: Theme.paddingMedium
            }

        }

        VerticalScrollDecorator {}
    }

}
