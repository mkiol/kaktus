/*
  Copyright (C) 2016-2025 Michal Kosciesza <michal@mkiol.net>

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
            return Orientation.PortraitMask;
        case 2:
            return Orientation.LandscapeMask;
        }
        return Orientation.All;
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
        Behavior on height {NumberAnimation { duration: 200; easing.type: Easing.OutQuad }}
        clip: true

        contentHeight: content.height

        Column {
            id: content

            width: root.width
            spacing: Theme.paddingMedium

            PageHeader {
                title: qsTr("Changelog")
            }

            SectionHeader {
                text: qsTr("Version %1").arg("3.2")
            }

            LogItem {
                title: "Netvibes removed"
                description: "Netvibes feed reader service has been discontinued and support for it in Kaktus has been removed."
            }

            LogItem {
                title: "Fix: Scrolling in web view is broken"
                description: "Sometimes it was not possible to scroll through the website in web view. This error has been fixed."
            }

            SectionHeader {
                text: qsTr("Version %1").arg("3.1")
            }

            LogItem {
                title: "Improved web viewer"
                description: "Old WebKit based web view is replaced with new Sailfish WebView based on Gecko engine. " +
                             "Thanks to this change many web rendering bugs disappeared and html pages look much better now."
            }

            LogItem {
                title: "Reader View update"
                description: "Engine behind Reader View (Mozilla's Readability.js lib) has been " +
                             "updated to the most recent version."
            }

            LogItem {
                title: "Sandboxing"
                description: "SailJail is now explicitly enabled with " +
                             "following permissions needed: Internet, Pictures, WebView."
            }

            SectionHeader {
                text: qsTr("Version %1").arg("3.0.4")
            }

            LogItem {
                title: "Netvibes fixes"
                description: "Syncing from Netvibies was broken due to API change."
            }

            LogItem {
                title: "Translations update"
                description: "Czech and French translations were updated."
            }

            SectionHeader {
                text: qsTr("Version %1").arg("3.0.3")
            }

            LogItem {
                title: "Better Netvibes icons"
                description: "Low resolution Netvibes icons were replaced with improved ones."
            }

            SectionHeader {
                text: qsTr("Version %1").arg("3.0.2")
            }

            LogItem {
                title: "Mark as read/unread gesture"
                description: "Items on the list can be marked as read/unread with a swipe gesture. " +
                             "Many thanks to Renaud Casenave-Péré for the implementation."
            }

            LogItem {
                title: "Inverted screen orientations enabled"
                description: "Inverted landscape screen orientation is now possible. Thanks to this change " +
                             "Kaktus on Gemini PDA is more pleasant to use."
            }

            LogItem {
                title: "Fixes for bugs discovered on SFOS 4.0"
                description: "In the recent SFOS version certain app pages couldn't be loaded." +
                             "This issue is now resolved."
            }

            Spacer {}
        }
    }

    VerticalScrollDecorator {
        flickable: flick
    }
}
