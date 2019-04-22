/*
  Copyright (C) 2016-2019 Michal Kosciesza <michal@mkiol.net>

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
            return Orientation.Portrait
        case 2:
            return Orientation.Landscape
        }
        return Orientation.Landscape | Orientation.Portrait
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
                text: qsTr("Version %1").arg("3.0.0")
            }

            LogItem {
                title: "Tiny Tiny RSS"
                description: "Tiny Tiny RSS is now supported as a new feeds " +
                             "aggregator. Tiny Tiny RSS is a free and " +
                             "open source web-based news feed (RSS/Atom) reader. " +
                             "To find more about it, please check the official " +
                             "<a href=\"https://tt-rss.org/\">web page</a>.";
            }

            LogItem {
                title: "UI refresh"
                description: "The app UI has been updated to match the newest " +
                             "Sailfish OS style. Many small elements of " +
                             "Tabs, Feeds and Aricles lists have been improved, " +
                             "together with images presentation and UI colors. " +
                             "Support for Light themes has been added as well."
            }

            LogItem {
                title: "Save images to gallery"
                description: "If article item contains Image, it can be " +
                             "saved to the gallery. The option to do so is " +
                             "located in the item's context menu."
            }

            LogItem {
                title: "Translations update"
                description: "Translations for Spanish, Chinese, German, " +
                             "Belgian Dutch and Netherlands Dutch have been updated. " +
                             "Many thanks to Carmen Fernández B., Rui Kon, " +
                             "qwer_asew, Nathan Follens and Heimen Stoffels.";
            }

            SectionHeader {
                text: qsTr("Version %1").arg("2.6.1")
            }

            LogItem {
                title: "Support for OnePlus X"
                description: "Missing icons on OnePlus X have beed fixed.";
            }

            SectionHeader {
                text: qsTr("Version %1").arg("2.6.0")
            }

            LogItem {
                title: "Pocket integration"
                description: "Pocket is an Internet tool for saving articles to read later. " +
                             "Integration implemented in Kaktus provides \"Add to Pocket\" " +
                             "button in the articles list and in the web viewer.";
            }

            LogItem {
                title: "Share link"
                description: "\"Share link\" button has been added. " +
                             "Due to Jolla Store restrictions it will be " +
                             "enabled only in OpenRepos package.";
            }

            LogItem {
                title: "Improved app icon"
                description: "Kaktus icon has a new fresh look!"
            }

            LogItem {
                title: "Delete web viewer cookies"
                description: "Option in the settings that allows you to clear " +
                             "cache and cookies of the web viewer."
            }

            Spacer {}
        }
    }

    VerticalScrollDecorator {
        flickable: flick
    }
}
