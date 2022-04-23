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
            anchors {
                left: parent.left
                right: parent.right
            }

            spacing: Theme.paddingMedium

            PageHeader {
                title: qsTr("Settings")
            }

            Row {
                anchors { right: parent.right; rightMargin: Theme.horizontalPageMargin}
                spacing: Theme.paddingMedium
                height: Math.max(icon.height, label.height)

                Image {
                    id: icon
                    anchors.verticalCenter: parent.verticalCenter
                    source: app.isNetvibes ? "image://icons/icon-m-netvibes" :
                        app.isOldReader ? "image://icons/icon-m-oldreader" :
                        app.isTTRss ? "image://icons/icon-m-ttrss" : null
                    width: Theme.iconSizeMedium
                    height: Theme.iconSizeMedium
                }

                Label {
                    id: label
                    anchors.verticalCenter: parent.verticalCenter
                    text: app.isNetvibes ? "Netvibes":
                        app.isOldReader ? "Old Reader" :
                        app.isTTRss ? "Tiny Tiny RSS" : null
                    color: Theme.highlightColor
                    font.pixelSize: Theme.fontSizeSmall
                }
            }

            ListItem {
                id: signinForm
                contentHeight: flow1.height + 2*Theme.paddingLarge

                Flow {
                    id: flow1
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left; anchors.right: parent.right
                    spacing: Theme.paddingMedium
                    anchors.leftMargin: Theme.paddingLarge
                    anchors.rightMargin: Theme.paddingLarge

                    Label {
                        text: qsTr("Not signed in")
                        visible: !settings.signedIn
                    }
                    Label {
                        text: settings.signinType == 1 || settings.signinType == 2 ?
                                  qsTr("Signed in with") : qsTr("Signed in as")
                        visible: settings.signedIn
                    }
                    Label {
                        color: Theme.highlightColor
                        visible: settings.signedIn
                        text: settings.signedIn ?
                            (settings.signinType == 1 ? "Twitter" :
                             settings.signinType == 2 ? "Facebook" :
                             settings.getUsername()) : ""
                    }
                }

                menu: ContextMenu {
                    MenuItem {
                        text: settings.signedIn ? qsTr("Sign out") : qsTr("Sign in")
                        onClicked: {
                            if (settings.signedIn) {
                                pageStack.push(Qt.resolvedUrl("SignOutDialog.qml"));
                            } else {
                                pageStack.push(Qt.resolvedUrl("SignInDialog.qml"),{"code": 0});
                            }
                        }
                    }
                }

                onClicked: openMenu();
            }

            ListItem {
                id: defaultdashboard
                contentHeight: visible ? flow2.height + 2*Theme.paddingLarge : 0
                enabled: settings.signedIn && utils.defaultDashboardName()!=="" && settings.signinType<10
                visible: app.isNetvibes

                Flow {
                    id: flow2
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left; anchors.right: parent.right
                    spacing: Theme.paddingMedium
                    anchors.leftMargin: Theme.paddingLarge
                    anchors.rightMargin: Theme.paddingLarge

                    Label {
                        color: settings.signedIn && utils.defaultDashboardName()!=="" ? Theme.primaryColor : Theme.secondaryColor
                        text: settings.signedIn && utils.defaultDashboardName()!=="" ? qsTr("Dashboard in use") : qsTr("Dashboard not selected")
                    }

                    Label {
                        id: dashboard
                        color: Theme.highlightColor
                        text: utils.defaultDashboardName()
                    }
                }

                onClicked: openMenu();

                menu: ContextMenu {
                    MenuItem {
                        text: qsTr("Change")
                        onClicked: {
                            utils.setDashboardModel();
                            pageStack.push(Qt.resolvedUrl("DashboardDialog.qml"));
                        }
                    }
                }
            }

            ExpandingSectionGroup {
                ExpandingSection {
                    title: qsTr("Syncronization")

                    content.sourceComponent: Column {
                        ComboBox {
                            width: root.width
                            label: qsTr("Sync timeframe")
                            enabled: settings.signinType>=10
                            visible: enabled

                            currentIndex: {
                                var retention = settings.getRetentionDays();
                                if (retention < 1)
                                    return 5;
                                if (retention < 3)
                                    return 0;
                                if (retention < 7)
                                    return 1;
                                if (retention < 14)
                                    return 2;
                                if (retention < 30)
                                    return 3;
                                return 4;
                            }

                            menu: ContextMenu {
                                MenuItem { text: qsTr("1 Day") }
                                MenuItem { text: qsTr("3 Days") }
                                MenuItem { text: qsTr("1 Week") }
                                MenuItem { text: qsTr("2 Weeks") }
                                MenuItem { text: qsTr("1 Month") }
                                MenuItem { text: qsTr("Wide as possible") }
                            }

                            onCurrentIndexChanged: {
                                if (currentIndex == 0) {
                                    settings.setRetentionDays(1);
                                    return;
                                }
                                if (currentIndex == 1) {
                                    settings.setRetentionDays(3);
                                    return;
                                }
                                if (currentIndex == 2) {
                                    settings.setRetentionDays(7);
                                    return;
                                }
                                if (currentIndex == 3) {
                                    settings.setRetentionDays(14);
                                    return;
                                }
                                if (currentIndex == 4) {
                                    settings.setRetentionDays(30);
                                    return;
                                }
                                settings.setRetentionDays(0);
                            }

                            description: qsTr("Most recent articles will be syncronized according to the defined timeframe.") + " " +
                                         (settings.signinType < 20 ? qsTr("Regardless of the value, all starred, liked and shared items will be synced as well.") : qsTr("Regardless of the value, all saved items will be synced as well.")) + " " +
                                         qsTr("Be aware, this parameter has significant impact on the speed of synchronization.")
                        }

                        TextSwitch {
                            id: syncReadSwitch
                            text: qsTr("Sync read articles")
                            description: qsTr("In addition to unread also read articles will be synced. " +
                                              "Disabling this option will speed up synchronization, but read articles will not be accessible form Kaktus.");
                            onCheckedChanged: {
                                settings.syncRead = checked;
                            }
                            Component.onCompleted: {
                                checked = settings.syncRead;
                            }
                        }

                        Spacer {}
                    }
                }

                ExpandingSection {
                    title: qsTr("Cache")

                    content.sourceComponent: Column {
                        ListItem {
                            contentHeight: flow3.height + 2*Theme.paddingLarge
                            enabled: true

                            onClicked: openMenu()

                            Flow {
                                id: flow3
                                spacing: Theme.paddingMedium
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.left: parent.left; anchors.right: parent.right
                                anchors.leftMargin: Theme.paddingLarge
                                anchors.rightMargin: Theme.paddingLarge

                                Label {
                                    text: qsTr("Current cache size")
                                }

                                Label {
                                    color: Theme.secondaryColor
                                    text: utils.getHumanFriendlySizeString(dm.cacheSize);
                                }
                            }

                            menu: ContextMenu {
                                MenuItem {
                                    text: qsTr("Delete cache")
                                    onClicked: {
                                        fetcher.cancel(); dm.cancel();
                                        dm.removeCache();
                                    }
                                }
                            }
                        }

                        ComboBox {
                            width: root.width
                            label: qsTr("Network mode")
                            currentIndex: settings.offlineMode ? 1 : 0

                            menu: ContextMenu {
                                MenuItem { text: qsTr("Online"); }
                                MenuItem { text: qsTr("Offline"); }
                            }

                            onCurrentIndexChanged: {
                                if (currentIndex==0)
                                    settings.offlineMode = false;
                                else
                                    settings.offlineMode = true;
                            }

                            description: qsTr("In offline mode, Kaktus will only use local cache to get web pages and images, so "+
                                              "network connection won't be needed.")
                        }

                        TextSwitch {
                            text: qsTr("Auto network mode")
                            description: qsTr("Network mode will be switched automatically on network connection lost or restore.")
                            onCheckedChanged: {
                                settings.autoOffline = checked;
                            }
                            Component.onCompleted: {
                                checked = settings.autoOffline;
                            }
                        }

                        ComboBox {
                            width: root.width
                            label: qsTr("Cache articles")
                            currentIndex: settings.cachingMode

                            menu: ContextMenu {
                                MenuItem { text: qsTr("Never") }
                                MenuItem { text: qsTr("WiFi only") }
                                MenuItem { text: qsTr("Always") }
                            }

                            onCurrentIndexChanged: {
                                settings.cachingMode = currentIndex;
                            }

                            description: qsTr("After sync the content of all items will be downloaded "+
                                              "and cached for access in the offline mode.")
                        }

                        Spacer {}
                    }
                }

                ExpandingSection {
                    title: qsTr("Web viewer")

                    content.sourceComponent: Column {
                        ComboBox {
                            width: root.width
                            label: qsTr("Open link behaviour")
                            currentIndex: settings.webviewNavigation

                            menu: ContextMenu {
                                MenuItem { text: qsTr("Disabled") }
                                MenuIconItem { text: qsTr("External browser"); iconSource: "image://icons/icon-m-browser?" + Theme.primaryColor }
                                MenuIconItem { text: qsTr("Web viewer"); iconSource: "image://icons/icon-m-webview?" + Theme.primaryColor }
                            }

                            onCurrentIndexChanged: {
                                settings.webviewNavigation = currentIndex;
                            }

                            description: qsTr("Defines how navigation is handled inside built-in web viewer. Hyperlinks could be disabled, opened in an external browser or opened inside web viewer.")
                        }

                        TextSwitchWithIcon {
                            text: qsTr("Auto switch to Reader View")
                            description: qsTr("Reader View is a feature that strips away clutter like buttons, ads and background images, and changes the page's layout for better readability. By enabling this option, Reader View will be automatically switch on when page is loaded in the web viewer.")
                            iconSource: settings.readerMode ? "image://icons/icon-m-reader-selected?" + Theme.primaryColor : "image://icons/icon-m-reader?" + Theme.primaryColor
                            onCheckedChanged: {
                                settings.readerMode = checked;
                            }
                            Component.onCompleted: {
                                checked = settings.readerMode;
                            }
                        }

                        /*ComboBox {
                            width: root.width
                            label: qsTr("Reader View theme")
                            description: qsTr("Style of theme which will be used to display articles in Reader View.")
                            currentIndex: {
                                if (settings.readerTheme === "dark")
                                    return 0;
                                if (settings.readerTheme === "light")
                                    return 1;
                            }

                            menu: ContextMenu {
                                MenuItem { text: qsTr("Dark") }
                                MenuItem { text: qsTr("Light") }
                            }

                            onCurrentIndexChanged: {
                                switch (currentIndex) {
                                case 0:
                                    settings.readerTheme = "dark";
                                    break;
                                case 1:
                                    settings.readerTheme = "light";
                                    break;
                                }
                            }
                        }*/

                        TextSwitchWithIcon {
                            text: qsTr("Auto switch to Night View")
                            description: qsTr("Night View reduces the brightness of websites. By enabling this option, Night View will be automatically switch on when page is loaded in the web viewer.")
                            iconSource: settings.nightMode ? "image://icons/icon-m-night-selected?" + Theme.primaryColor : "image://icons/icon-m-night?" + Theme.primaryColor
                            onCheckedChanged: {
                                settings.nightMode = checked;
                            }
                            Component.onCompleted: {
                                checked = settings.nightMode;
                            }
                        }

                        IconSlider {
                            leftIconSource: "image://icons/icon-m-fontdown?" + Theme.primaryColor
                            rightIconSource: "image://icons/icon-m-fontup?" + Theme.primaryColor
                            label: qsTr("Viewer font size level")
                            minimumValue: 50
                            maximumValue: 200
                            value: Math.floor(settings.zoom * 100)
                            valueText: value + "%"
                            stepSize: 10
                            onValueChanged: settings.zoom = value/100
                            onClicked: {
                                // Default value
                                value = 100;
                            }
                        }

                        ButtonItem {
                            button.text: qsTr("Delete cookies")
                            description: qsTr("Clear web viewer cache and cookies. Changes will take effect after restart.")
                            button.onClicked: {
                                utils.resetQtWebKit()
                                notification.show(qsTr("Cache and cookies have been deleted"))
                            }
                        }

                        Spacer {}
                    }
                }

                ExpandingSection {
                    title: qsTr("UI")

                    content.sourceComponent: Column {
                        ComboBox {
                            id: locale
                            width: root.width
                            label: qsTr("Language")
                            currentIndex: {
                                if (settings.locale === "en")
                                    return 1;
                                return 0;
                            }

                            menu: ContextMenu {
                                MenuItem { text: qsTr("Default"); onClicked: locale.showMessage() }
                                MenuItem { text: "English"; onClicked: locale.showMessage() }
                            }

                            onCurrentIndexChanged: {
                                switch (currentIndex) {
                                case 0:
                                    settings.locale = "";
                                    break;
                                case 1:
                                    settings.locale = "en";
                                    break;
                                }
                            }

                            function showMessage() {
                                notification.show(qsTr("Changes will take effect after you restart Kaktus"));
                            }
                        }

                        ComboBox {
                            width: root.width
                            label: qsTr("View mode")
                            currentIndex: {
                                switch (settings.viewMode) {
                                case 0:
                                    return 0;
                                case 1:
                                    return 1;
                                case 3:
                                    return 2;
                                case 4:
                                    return 3;
                                case 5:
                                case 6:
                                    return 4;
                                }
                            }

                            menu: ContextMenu {
                                MenuIconItem {
                                    text: app.isNetvibes ? qsTr("Tabs, feeds & articles") : qsTr("Folders, feeds & articles")
                                    iconSource: "image://icons/icon-m-vm0?" + Theme.primaryColor
                                }
                                MenuIconItem {
                                    text: app.isNetvibes ? qsTr("Tabs & articles") : qsTr("Folders & articles")
                                    iconSource: "image://icons/icon-m-vm1?" + Theme.primaryColor
                                }
                                MenuIconItem {
                                    text: qsTr("All articles")
                                    iconSource: "image://icons/icon-m-vm3?" + Theme.primaryColor
                                }
                                MenuIconItem {
                                    text: app.isNetvibes ? qsTr("Saved") : qsTr("Starred")
                                    iconSource: "image://icons/icon-m-vm4?" + Theme.primaryColor
                                }
                                MenuIconItem {
                                    visible: !app.isTTRss
                                    enabled: app.isNetvibes || (app.isOldReader && settings.showBroadcast)
                                    text: app.isNetvibes ? qsTr("Slow") : qsTr("Liked")
                                    iconSource: app.isNetvibes ? "image://icons/icon-m-vm5?" + Theme.primaryColor : "image://icons/icon-m-vm6?" + Theme.primaryColor
                                }
                            }

                            onCurrentIndexChanged: {
                                switch (currentIndex) {
                                case 0:
                                    settings.viewMode = 0; break;
                                case 1:
                                    settings.viewMode = 1; break;
                                case 2:
                                    settings.viewMode = 3; break;
                                case 3:
                                    settings.viewMode = 4; break;
                                case 4:
                                    if (app.isNetvibes)
                                        settings.viewMode = 5;
                                    else
                                        settings.viewMode = 6;
                                    break;
                                }
                            }
                        }

                        ComboBox {
                            width: root.width
                            label: qsTr("Sort order for list of articles")
                            currentIndex: settings.showOldestFirst ? 1 : 0

                            menu: ContextMenu {
                                MenuItem { text: qsTr("Recent first") }
                                MenuItem { text: qsTr("Oldest first") }
                            }

                            onCurrentIndexChanged: {
                                switch (currentIndex) {
                                case 0:
                                    settings.showOldestFirst = false; break;
                                case 1:
                                    settings.showOldestFirst = true; break;
                                }
                            }
                        }

                        ComboBox {
                            width: root.width
                            label: qsTr("Clicking on article behaviour")
                            currentIndex: settings.clickBehavior

                            menu: ContextMenu {
                                MenuIconItem { text: qsTr("Web viewer"); iconSource: "image://icons/icon-m-webview?" + Theme.primaryColor }
                                MenuIconItem { text: qsTr("External browser"); iconSource: "image://icons/icon-m-browser?" + Theme.primaryColor }
                                MenuIconItem { text: qsTr("Feed content"); iconSource: "image://icons/icon-m-rss?" + Theme.primaryColor }
                            }

                            onCurrentIndexChanged: {
                                settings.clickBehavior = currentIndex;
                            }

                            description: qsTr("Defines the behavior for clicking on an article item. Article can be opened in the built-in web viewer, opened in an external browser or full RSS feed content can be shown.")
                        }

                        ComboBox {
                            width: root.width
                            label: qsTr("List filtering")
                            currentIndex: settings.filter

                            menu: ContextMenu {
                                MenuIconItem { text: qsTr("All articles"); iconSource: "image://icons/icon-m-filter-0?" + Theme.primaryColor }
                                MenuIconItem { text: app.isNetvibes ? qsTr("Unread or saved") : qsTr("Unread or starred"); iconSource: "image://icons/icon-m-filter-1?" + Theme.primaryColor }
                                MenuIconItem { text: qsTr("Only unread"); iconSource: "image://icons/icon-m-filter-2?" + Theme.primaryColor }
                            }

                            onCurrentIndexChanged: {
                                settings.filter = currentIndex;
                            }

                            description: app.isNetvibes ?
                                             qsTr("List of articles can be filtered to display all articles, unread and saved or only unread.") :
                                             qsTr("List of articles can be filtered to display all articles, unread and starred or only unread.")
                        }

                        TextSwitch {
                            text: qsTr("Social features")
                            enabled: app.isOldReader
                            description: qsTr("Following Old Reader's social features will be enabled: Following folder, Sharing article with followers, Like option, Liked articles view mode.")
                            onCheckedChanged: {
                                settings.showBroadcast = checked;
                            }
                            Component.onCompleted: {
                                checked = settings.showBroadcast;
                            }
                        }

                        TextSwitch {
                            text: qsTr("Expanded items")
                            description: qsTr("All article items on the list view be shown expanded.")
                            onCheckedChanged: {
                                settings.expandedMode = checked;
                            }
                            Component.onCompleted: {
                                checked = settings.expandedMode;
                            }
                        }

                        TextSwitch {
                            text: app.isTablet ? qsTr("Double-pane reader") : qsTr("Double-pane reader in landscape")
                            description: app.isTablet ? qsTr("View with the articles will be splited in to two colums.") : qsTr("View with the articles in the landscape orientation will be splited in to two colums.")
                            onCheckedChanged: {
                                settings.doublePane = checked;
                            }
                            Component.onCompleted: {
                                checked = settings.doublePane;
                            }
                        }

                        /*TextSwitch {
                            text: qsTr("Power save mode")
                            description: qsTr("When the phone or app goes to the idle state, "+
                                              "all opened web pages will be closed to lower power consumption.")
                            onCheckedChanged: {
                                settings.powerSaveMode = checked;
                            }
                            Component.onCompleted: {
                                checked = settings.powerSaveMode;
                            }
                        }*/

                        ComboBox {
                            width: root.width
                            label: qsTr("Orientation")
                            currentIndex: settings.allowedOrientations

                            menu: ContextMenu {
                                MenuItem { id: allOrientations; text: qsTr("Dynamic") }
                                MenuItem { id: portraitOrientation; text: qsTr("Portrait") }
                                MenuItem { id: landscapeOrientation; text: qsTr("Landscape") }
                            }

                            onCurrentIndexChanged: settings.allowedOrientations = currentIndex
                        }

                        Spacer {}
                    }
                }

                ExpandingSection {
                    title: qsTr("Pocket")

                    content.sourceComponent: Column {
                        TextSwitchWithIcon {
                            text: qsTr("Pocket integration")
                            description: qsTr("Pocket is an Internet tool for saving articles to read later. Integration implemented in Kaktus provides \"Add to Pocket\" button in the articles list and in the web viewer.")
                            iconSource: "image://icons/icon-m-pocket?" + Theme.primaryColor
                            checked: settings.pocketEnabled
                            busy: pocket.busy
                            enabled: dm.online
                            automaticCheck: false
                            onClicked: {
                                if (checked) {
                                    settings.pocketEnabled = false
                                } else {
                                    pocket.enable()
                                }
                            }
                        }

                        TextFieldItem {
                            id: pocketTagsField
                            enabled: settings.pocketEnabled
                            textField.placeholderText: qsTr("Default tags")
                            textField.label: qsTr("Default tags")
                            textField.labelVisible: false
                            description: qsTr("List of comma seperated tags that will be automatically inserted when you add article to Pocket.")
                            textField.inputMethodHints: Qt.ImhNoAutoUppercase
                            textField.onTextChanged: timer.restart()
                            Component.onCompleted: textField.text = settings.pocketTags

                            EnterKey.iconSource: "image://theme/icon-m-enter-close"
                            EnterKey.onClicked: {
                                checkTags()
                                parent.focus = true
                            }

                            onFocusChanged: {
                                if (!focus)
                                    checkTags()
                            }

                            function checkTags() {
                                timer.stop()
                                pocketTagsField.textField.text = pocket.fixTags(pocketTagsField.textField.text)
                                settings.pocketTags = pocketTagsField.textField.text
                            }

                            Timer {
                                id: timer
                                interval: 1000
                                onTriggered: pocketTagsField.checkTags()
                            }
                        }

                        TextSwitch {
                            text: qsTr("Quick adding")
                            description: qsTr("If enabled, article will be send to Pocket immediately after you click on \"Add to Pocket\" button, so without any confirmation dialog. All tags from \"Default tags\" field will be automatically added.")
                            checked: settings.pocketQuickAdd
                            enabled: settings.pocketEnabled
                            onCheckedChanged: {
                                settings.pocketQuickAdd = checked
                            }
                        }

                        ButtonItem {
                            enabled: settings.pocketEnabled
                            button.text: qsTr("Delete saved tags")
                            button.onClicked: {
                                settings.pocketTagsHistory = ""
                                notification.show(qsTr("Saved tags have been deleted"))
                            }
                        }

                        Spacer {}
                    }
                }

                ExpandingSection {
                    title: qsTr("Other")

                    content.sourceComponent: Column {
                        TextSwitch {
                            text: qsTr("Ignore SSL errors")
                            onCheckedChanged: settings.ignoreSslErrors = checked;
                            Component.onCompleted: checked = settings.ignoreSslErrors
                        }

                        Spacer {}
                    }
                }
            }
        }
    }

    VerticalScrollDecorator {
        flickable: flick
    }
}

