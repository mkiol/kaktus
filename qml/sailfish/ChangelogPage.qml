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
                text: qsTr("Version %1").arg("2.3")
            }

            LogItem {
                title: 'Full content from the RSS feed'
                description: "New option to set 'click on article' action. Following actions are possible: Open article in the build-in viewer, Open article in the external browser, Show full content from the RSS feed.";
            }

            SectionHeader {
                text: qsTr("Version %1").arg("2.2")
            }

            LogItem {
                title: 'Initial tablet support'
                description: "Jolla Tablet is now supported.";
            }

            LogItem {
                title: 'Double-pane landscape view'
                description: "Landscape view is rearanged to display content on two panes.";
            }

            LogItem {
                title: 'Open articles in an external browser'
                description: "Context menu option to open article directly in an external web browser instead built-in web viewer.";
            }

            LogItem {
                title: 'Icon-based context menu'
                description: "Context menu can be icon-based (default) or text-based. Inspiration comes form gPodder app.";
            }

            LogItem {
                title: 'Updated Netvibes API support'
                description: "Fixes for updated NV API including, long-awaited, SSL support.";
            }

            LogItem {
                title: 'UI improvements'
                description: "Some small UI changes to better fit in to new Sailfish 2.0 design style.";
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


            Item {
                height: Theme.paddingMedium
            }

        }

        VerticalScrollDecorator {}
    }

}
