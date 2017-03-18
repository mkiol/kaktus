/*
  Copyright (C) 2016 Michal Kosciesza <michal@mkiol.net>

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

    SilicaFlickable {
        id: flick
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        height: app.flickHeight
        clip: true
        contentHeight: content.height

        Column {
            id: content
            anchors {
                left: parent.left
                right: parent.right
            }

            spacing: Theme.paddingMedium

            PageHeader {
                title: qsTr("Changelog")
            }

            SectionHeader {
                text: qsTr("Version %1").arg("2.6.0")
            }

            LogItem {
                title: "Pocket integration"
                description: "Pocket is an Internet tool for saving articles to read later. Integration implemented in Kaktus provides \"Add to Pocket\" button in the articles list and in the web viewer.";
            }

            LogItem {
                title: "Share link"
                description: "\"Share link\" button has been added. Due to Jolla Store restrictions it will be enabled only in OpenRepos package.";
            }

            LogItem {
                title: "Improved app icon"
                description: "Kaktus icon has a new fresh look!"
            }

            LogItem {
                title: "Delete web viewer cookies"
                description: "Option in the settings that allows you to clear cache and cookies of the web viewer."
            }

            LogItem {
                title: "Spanish translation update"
                description: "Spanish translations has been updated."
            }

            SectionHeader {
                text: qsTr("Version %1").arg("2.5.3")
            }

            LogItem {
                title: 'Bug fixes for Netvibes'
                description: "Bug related to syncing process in Netvibes has been fixed.";
            }

            SectionHeader {
                text: qsTr("Version %1").arg("2.5.2")
            }

            LogItem {
                title: 'Bug fixes'
                description: "Some bugs related to caching process have been fixed.";
            }

            SectionHeader {
                text: qsTr("Version %1").arg("2.5.1")
            }

            LogItem {
                title: 'Updated Netvibes API'
                description: "Fixes for updated Netvibes API.";
            }

            SectionHeader {
                text: qsTr("Version %1").arg("2.5.0")
            }

            LogItem {
                title: 'Reader View'
                description: "Reader View is a feature that strips away clutter like buttons, ads and background images, and changes the page's layout for better readability. Reader View implementation in Kaktus is based on Readability.js library, the same that is used in Firefox browser.";
            }

            LogItem {
                title: 'UI redesign'
                description: "Some options were moved from pull down menu to the bottom bar and bottom bar has a new dark look.";
            }

            LogItem {
                title: 'Unsynced data indicator'
                description: "When Kaktus has any unsynchronized data, indicator (red dot) is shown on the bottom bar.";
            }

            LogItem {
                title: 'Smoother offline mode'
                description: "A few bugs were fixed and general offline mode experience has been improved.";
            }

            LogItem {
                title: 'List filtering'
                description: "List of articles can be filtered to display all articles, unread and saved or only unread.";
            }

            LogItem {
                title: 'Auto network mode'
                description: "Option to automatically enabling offline mode on network connection lost.";
            }

            LogItem {
                title: 'Night View'
                description: "Night View reduces the brightness of websites by inverting colors (heavily inspired and code partially borrowed from harbour-webpirate project).";
            }

            SectionHeader {
                text: qsTr("Version %1").arg("2.4.0")
            }

            LogItem {
                title: 'UI improvements for Jolla C/Aqua Fish and other devices'
                description: "Few UI fixes to better support other devices than Jolla 1.";
            }

            SectionHeader {
                text: qsTr("Version %1").arg("2.3.1")
            }

            LogItem {
                title: 'Updated support for Netvibes API'
                description: "Fixes for updated Netvibes API.";
            }

            SectionHeader {
                text: qsTr("Version %1").arg("2.3")
            }

            LogItem {
                title: 'Full content from the RSS feed'
                description: "New option to set clicking on article behavior. Following actions are possible: Open article in the built-in viewer, Open article in an external browser, Show full content from the RSS feed.";
            }

            LogItem {
                title: 'Expanded items'
                description: "New option to always show all article items on the list view expanded.";
            }

            LogItem {
                title: 'Open link behaviour'
                description: "New option to change how navigation is handled inside built-in viewer.";
            }

            LogItem {
                title: 'UI improvements'
                description: "Many UI changes were made to improve user experience.";
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

            /*SectionHeader {
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
            }*/


            Spacer {}
        }
    }

    VerticalScrollDecorator {
        flickable: flick
    }
}
