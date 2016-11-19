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

    property string title
    property string content
    property string entryId
    property string offlineUrl
    property string onlineUrl
    property bool stared
    property bool read
    property int index
    property int feedindex
    property bool cached

    allowedOrientations: {
        switch (settings.allowedOrientations) {
        case 1:
            return Orientation.Portrait;
        case 2:
            return Orientation.Landscape;
        }
        return Orientation.Landscape | Orientation.Portrait;
    }

    Component.onCompleted: {
        bar.hide();
        controlbar.show();
    }

    function check() {
        // Not allowed while Syncing
        if (dm.busy || fetcher.busy || dm.removerBusy) {
            notification.show(qsTr("Wait until current task is complete."));
            return false;
        }

        // Entry not cached and offline mode enabled
        if (settings.offlineMode && !cached) {
            notification.show(qsTr("Offline version not available."));
            return false;
        }

        // Switch to Offline mode if no network
        if (!settings.offlineMode && !dm.online) {
            if (cached) {
                // Entry cached
                notification.show(qsTr("Enabling offline mode because network is disconnected."));
                settings.offlineMode = true;
            } else {
                // Entry not cached
                notification.show(qsTr("Network is disconnected."));
                return false;
            }
        }

        return true;
    }

    function openEntryInBrowser() {
        entryModel.setData(index, "read", 1, "");
        notification.show(qsTr("Launching an external browser."));
        Qt.openUrlExternally(settings.offlineMode ? offlineUrl : onlineUrl);
    }

    function openEntryInViewer() {

        // (!dm.online && settings.offlineMode) -> WORKAROUND for https://github.com/mkiol/kaktus/issues/14
        if (!dm.online && settings.offlineMode) {
            openEntryInBrowser();
            return;
        }

        pageStack.replace(Qt.resolvedUrl("WebPreviewPage.qml"),
                       {"entryId": entryId,
                           "onlineUrl": onlineUrl,
                           "offlineUrl": offlineUrl,
                           "title": title,
                           "stared": stared,
                           "index": index,
                           "feedindex": feedindex,
                           "read" : read,
                           "cached" : cached
                       });
    }

    function openEntry() {

        if (!check()) {
            return;
        }

        if (settings.clickBehavior === 1) {
            openEntryInBrowser();
            return;
        }

        openEntryInViewer();
    }

    SilicaFlickable {
        id: view

        anchors.fill: parent
        contentHeight: column.height
        contentWidth: parent.width

        Column {
            id: column

            anchors.left: parent.left; anchors.right: parent.right
            spacing: Theme.paddingLarge

            PageHeader {
                title: root.title
            }

            Label {
                anchors.left: parent.left; anchors.right: parent.right;
                anchors.leftMargin: Theme.horizontalPageMargin ; anchors.rightMargin: Theme.horizontalPageMargin
                text: root.content
                wrapMode: Text.Wrap
                textFormat: Text.StyledText
                linkColor: Theme.highlightColor
                font.pixelSize: Theme.fontSizeSmall
            }

            Item {
                height: 2 * Theme.paddingLarge
                width: parent.width
            }

            /*Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Open")
                onClicked: {
                    openEntry();
                }
            }

            Item {
                height: Theme.paddingLarge
                width: parent.width
            }*/
        }

        VerticalScrollDecorator {}
    }

    ControlBarWebPreview {
        id: controlbar
        flick: view
        canBack: false
        canMarkRead: true
        canStar: true
        canClipboard: true
        canReader: false
        canOpenWebview: true
        canOpenBrowser: !settings.openInBrowser
        stared: root.stared
        read: root.read
        transparent: false

        onBackClicked: pageStack.pop()

        onMarkReadClicked: {
            if (read) {
                read=false;
                entryModel.setData(root.index, "readr", 0, "");
            } else {
                read=true;
                entryModel.setData(root.index, "read", 1, "");
            }
        }

        onStarClicked: {
            if (stared) {
                stared=false;
                entryModel.setData(root.index, "readlater", 0, "");
            } else {
                stared=true;
                entryModel.setData(root.index, "readlater", 1, "");
            }
        }

        onWebviewClicked: {
            if (!check()) {
                return;
            }
            openEntryInViewer();
        }

        onBrowserClicked: {
            openEntryInBrowser();
        }

        onFontDownClicked: {
            if (settings.fontSize>0)
                settings.fontSize -= 1;
        }

        onFontUpClicked: {
            if (settings.fontSize<2)
                settings.fontSize += 1;
        }

        onClipboardClicked: {
            notification.show(qsTr("URL was copied to the clipboard."));
            Clipboard.text = root.onlineUrl;
        }
    }
}
