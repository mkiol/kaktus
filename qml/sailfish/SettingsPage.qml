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
            title: qsTr("Settings")
        }

        model: VisualItemModel {
            id: model
            Item {
                anchors { left: parent.left; right: parent.right}
                height: Math.max(icon.height, label.height)

                Image {
                    id: icon
                    anchors { right: label.left; rightMargin: Theme.paddingMedium }
                    source: app.isNetvibes ? "nv.png" :
                            app.isOldReader ? "oldreader.png" : "feedly.png"
                }

                Label {
                    id: label
                    anchors { right: parent.right; rightMargin: Theme.paddingLarge}
                    text: app.isNetvibes ? "Netvibes":
                          app.isOldReader ? "Old Reader" : "Feedly"
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignRight
                    color: Theme.highlightColor
                    font.pixelSize: Theme.fontSizeSmall
                    y: Theme.paddingSmall/2
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
                        text: qsTr("Not signed in ")
                        visible: !settings.signedIn
                    }
                    Label {
                        text: qsTr("Signed in with")
                        visible: settings.signedIn
                    }
                    Label {
                        color: Theme.highlightColor
                        visible: settings.signedIn
                        text: settings.signedIn ?
                                  settings.signinType==0 ? settings.getUsername() :
                                  settings.signinType==1 ? "Twitter" :
                                  settings.signinType==2 ? "Facebook" :
                                  settings.signinType==10 ? settings.getUsername() :
                                  settings.signinType==20 ? settings.getProvider() : "" : ""
                    }
                }

                menu: ContextMenu {
                    MenuItem {
                        text: settings.signedIn ? qsTr("Sign out") : qsTr("Sign in")
                        //enabled: settings.signedIn ? true : dm.online
                        onClicked: {
                            if (settings.signedIn) {
                                pageStack.push(Qt.resolvedUrl("SignOutDialog.qml"));
                            } else {
                                pageStack.push(Qt.resolvedUrl("SignInDialog.qml"),{"code": 0});
                            }
                        }
                    }
                }

                onClicked: showMenu();
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

                onClicked: showMenu();

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

            SectionHeader {
                text: qsTr("Syncronization")
            }

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

            SectionHeader {
                text: qsTr("Cache")
            }

            ListItem {
                contentHeight: flow3.height + 2*Theme.paddingLarge
                enabled: true

                onClicked: showMenu()

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
                    MenuItem { id: onlineMode; text: qsTr("Online") }
                    MenuItem { id: offlineMode; text: qsTr("Offline") }
                }

                onCurrentIndexChanged: {
                    if (currentIndex==0)
                        settings.offlineMode = false;
                    else
                        settings.offlineMode = true;
                }

                description: qsTr("In the offline mode, Kaktus will only use local cache to get web pages and images, so "+
                                  "network connection won't be needed.")
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

            SectionHeader {
                text: qsTr("UI")
            }

            ComboBox {
                id: locale
                width: root.width
                label: qsTr("Language")
                currentIndex: {
                    if (settings.locale === "")
                        return 0;
                    if (settings.locale === "cs")
                        return 1;
                    if (settings.locale === "en")
                        return 2;
                    if (settings.locale === "es")
                        return 3;
                    if (settings.locale === "it")
                        return 4;
                    if (settings.locale === "nl")
                        return 5;
                    if (settings.locale === "pl")
                        return 6;
                    if (settings.locale === "ru")
                        return 7;
                    if (settings.locale === "tr")
                        return 8;
                    return 0;
                }

                menu: ContextMenu {
                    MenuItem { text: qsTr("Default"); onClicked: locale.showMessage() }
                    MenuItem { text: "Čeština"; onClicked: locale.showMessage() }
                    MenuItem { text: "English"; onClicked: locale.showMessage() }
                    MenuItem { text: "Espanol"; onClicked: locale.showMessage() }
                    MenuItem { text: "Italiano"; onClicked: locale.showMessage() }
                    MenuItem { text: "Nederlands"; onClicked: locale.showMessage() }
                    MenuItem { text: "Polski"; onClicked: locale.showMessage()  }
                    MenuItem { text: "Русский"; onClicked: locale.showMessage() }
                    MenuItem { text: "Türkçe"; onClicked: locale.showMessage() }
                }

                onCurrentIndexChanged: {
                    switch (currentIndex) {
                    case 0:
                        settings.locale = "";
                        break;
                    case 1:
                        settings.locale = "cs";
                        break;
                    case 2:
                        settings.locale = "en";
                        break;
                    case 3:
                        settings.locale = "es";
                        break;
                    case 4:
                        settings.locale = "it";
                        break;
                    case 5:
                        settings.locale = "nl";
                        break;
                    case 6:
                        settings.locale = "pl";
                        break;
                    case 7:
                        settings.locale = "ru";
                        break;
                    case 8:
                        settings.locale = "tr";
                        break;
                    }
                }

                function showMessage() {
                    notification.show(qsTr("Changes will take effect after you restart Kaktus."));
                }
            }

            /*ComboBox {
                id: locale
                width: root.width
                label: qsTr("Language")
                currentIndex: {
                    if (settings.locale === "")
                        return 0;
                    if (settings.locale === "cs")
                        return 1;
                    if (settings.locale === "de")
                        return 2;
                    if (settings.locale === "en")
                        return 3;
                    if (settings.locale === "es")
                        return 4;
                    if (settings.locale === "fa")
                        return 5;
                    if (settings.locale === "fi")
                        return 6;
                    if (settings.locale === "fr")
                        return 7;
                    if (settings.locale === "it")
                        return 8;
                    if (settings.locale === "nl")
                        return 9;
                    if (settings.locale === "pl")
                        return 10;
                    if (settings.locale === "ru")
                        return 11;
                    if (settings.locale === "tr")
                        return 12;
                    if (settings.locale === "zh_CN")
                        return 13;
                }

                menu: ContextMenu {
                    MenuItem { text: qsTr("Default"); onClicked: locale.showMessage() }
                    MenuItem { text: "Čeština"; onClicked: locale.showMessage() }
                    MenuItem { text: "Deutsch"; onClicked: locale.showMessage() }
                    MenuItem { text: "English"; onClicked: locale.showMessage() }
                    MenuItem { text: "Espanol"; onClicked: locale.showMessage() }
                    MenuItem { text: "فارسی"; onClicked: locale.showMessage() }
                    MenuItem { text: "Suomi"; onClicked: locale.showMessage() }
                    MenuItem { text: "Français"; onClicked: locale.showMessage() }
                    MenuItem { text: "Italiano"; onClicked: locale.showMessage() }
                    MenuItem { text: "Nederlands"; onClicked: locale.showMessage() }
                    MenuItem { text: "Polski"; onClicked: locale.showMessage()  }
                    MenuItem { text: "Русский"; onClicked: locale.showMessage() }
                    MenuItem { text: "Türkçe"; onClicked: locale.showMessage() }
                    MenuItem { text: "Čeština"; onClicked: locale.showMessage() }
                    MenuItem { text: "Čeština"; onClicked: locale.showMessage() }
                    MenuItem { text: "中文 (简体)"; onClicked: locale.showMessage() }
                }

                onCurrentIndexChanged: {
                    switch (currentIndex) {
                    case 0:
                        settings.locale = "";
                        break;
                    case 1:
                        settings.locale = "cs";
                        break;
                    case 2:
                        settings.locale = "de";
                        break;
                    case 3:
                        settings.locale = "en";
                        break;
                    case 4:
                        settings.locale = "es";
                        break;
                    case 5:
                        settings.locale = "fa";
                        break;
                    case 6:
                        settings.locale = "fi";
                        break;
                    case 7:
                        settings.locale = "fr";
                        break;
                    case 8:
                        settings.locale = "it";
                        break;
                    case 9:
                        settings.locale = "nl";
                        break;
                    case 10:
                        settings.locale = "pl";
                        break;
                    case 11:
                        settings.locale = "ru";
                        break;
                    case 12:
                        settings.locale = "tr";
                        break;
                    case 13:
                        settings.locale = "zh_CN";
                        break;
                    }
                }

                function showMessage() {
                    notification.show(qsTr("Changes will take effect after you restart Kaktus."));
                }
            }*/

            /*ComboBox {
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
                        return 4;
                    }
                }

                menu: ContextMenu {
                    MenuItem { text: settings.getSigninType()<10 ? qsTr("Tabs & Feeds") : qsTr("Folders & Feeds") }
                    MenuItem { text: settings.getSigninType()<10 ? qsTr("Only Folders") : qsTr("Only Folders") }
                    MenuItem { text: qsTr("All feeds") }
                    MenuItem { text: settings.getSigninType()<10 ? qsTr("Saved") : qsTr("Starred") }
                    MenuItem { text: qsTr("Slow") }
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
                        settings.viewMode = 5; break;
                    }
                }
            }*/

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
                label: qsTr("Context menu style")
                currentIndex: settings.iconContextMenu ? 0 : 1

                menu: ContextMenu {
                    MenuItem { text: qsTr("Icons") }
                    MenuItem { text: qsTr("Text") }
                }

                onCurrentIndexChanged: {
                    settings.iconContextMenu = (currentIndex == 0 ? true : false);
                }
            }

            ComboBox {
                width: root.width
                label: qsTr("Click on article action")
                currentIndex: settings.clickBehavior

                menu: ContextMenu {
                    MenuItem { text: qsTr("Open in viewer") }
                    MenuItem { text: qsTr("Open external browser") }
                    MenuItem { text: qsTr("Show feed content") }
                }

                onCurrentIndexChanged: {
                    settings.clickBehavior = currentIndex;
                }
            }

            TextSwitch {
                text: qsTr("Show only unread articles")
                onCheckedChanged: {
                    settings.showOnlyUnread = checked;
                }
                Component.onCompleted: {
                    checked = settings.showOnlyUnread;
                }
            }

            /*TextSwitch {
                text: qsTr("Open articles in browser")
                description: qsTr("Instead built-in web viewer, web pages will be opened in an external browser.")
                onCheckedChanged: {
                    settings.openInBrowser = checked;
                }
                Component.onCompleted: {
                    checked = settings.openInBrowser;
                }
            }

            TextSwitch {
                text: qsTr("Show content from the RSS feed")
                description: qsTr("Clicking on the article item will display new page with the entire content contained in an RSS feed.")
                onCheckedChanged: {
                    settings.showFeedContent = checked;
                }
                Component.onCompleted: {
                    checked = settings.showFeedContent;
                }
            }*/

            TextSwitchWithIcon {
                text: qsTr("Read mode")
                description: qsTr("Web pages will be reformatted into an easy to read version.")
                iconSource: settings.readerMode ? "reader.png" : "reader-disabled.png"
                onCheckedChanged: {
                    settings.readerMode = checked;
                }
                Component.onCompleted: {
                    checked = settings.readerMode;
                }
            }

            TextSwitch {
                text: qsTr("Show images")
                onCheckedChanged: {
                    settings.showTabIcons = checked;
                }
                Component.onCompleted: {
                    checked = settings.showTabIcons;
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
                text: qsTr("Power save mode")
                description: qsTr("When the phone or app goes to the idle state, "+
                                  "all opened web pages will be closed to lower power consumption.")
                onCheckedChanged: {
                    settings.powerSaveMode = checked;
                }
                Component.onCompleted: {
                    checked = settings.powerSaveMode;
                }
            }

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

            ComboBox {
                width: root.width
                label: qsTr("Offline viewer style")
                //description: qsTr("Style which will be used to display articles in the Offline mode.")
                currentIndex: {
                    if (settings.offlineTheme === "black")
                        return 0;
                    if (settings.offlineTheme === "white")
                        return 1;
                }

                menu: ContextMenu {
                    MenuItem { id: blackTheme; text: qsTr("Black") }
                    MenuItem { id: whiteTheme; text: qsTr("White") }
                }

                onCurrentIndexChanged: {
                    switch (currentIndex) {
                    case 0:
                        settings.offlineTheme = "black";
                        break;
                    case 1:
                        settings.offlineTheme = "white";
                        break;
                    }
                }
            }

            ComboBox {
                width: root.width
                label: qsTr("Web viewer font size")
                currentIndex: settings.fontSize

                menu: ContextMenu {
                    MenuItem { text: qsTr("-50%") }
                    MenuItem { text: qsTr("Normal") }
                    MenuItem { text: qsTr("+50%") }
                }

                onCurrentIndexChanged: {
                    settings.fontSize = currentIndex;
                }
            }

            SectionHeader {
                text: qsTr("Other")
            }

            Button {
                text: qsTr("Show User Guide")
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: {
                    guide.show();
                }
            }

            Item {
                height: Theme.paddingMedium
                width: height
            }

        }

        VerticalScrollDecorator {}
    }
}
