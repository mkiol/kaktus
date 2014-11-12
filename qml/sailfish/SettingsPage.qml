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

            SectionHeader {
                text: qsTr("Netvibes account")
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
                        text: qsTr("Signed in as")
                        visible: settings.signedIn
                    }
                    Label {
                        color: Theme.highlightColor
                        visible: settings.signedIn
                        text: settings.signedIn ? settings.getNetvibesUsername() : ""
                    }
                }

                menu: ContextMenu {
                    MenuItem {
                        text: settings.signedIn ? qsTr("Sign out") : qsTr("Sign in")
                        //enabled: settings.signedIn ? true : dm.online
                        onClicked: {
                            if (settings.signedIn) {
                                settings.signedIn = false;
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
                contentHeight: flow2.height + 2*Theme.paddingLarge
                enabled: settings.signedIn && utils.defaultDashboardName()!==""

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
            }

            TextSwitch {
                text: qsTr("Cache articles")
                description: qsTr("After sync the content of all items will be downloaded "+
                                  "and cached for access in the Offline mode.")
                Component.onCompleted: {
                    checked = settings.getAutoDownloadOnUpdate();
                }
                onCheckedChanged: {
                    settings.setAutoDownloadOnUpdate(checked);
                }
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
                    if (settings.locale === "fa")
                        return 3;
                    if (settings.locale === "nl")
                        return 4;
                    if (settings.locale === "pl")
                        return 5;
                    if (settings.locale === "ru")
                        return 6;
                }

                menu: ContextMenu {
                    MenuItem { text: qsTr("Default"); onClicked: locale.showMessage() }
                    MenuItem { text: "Čeština"; onClicked: locale.showMessage()  }
                    MenuItem { text: "English"; onClicked: locale.showMessage()  }
                    MenuItem { text: "فارسی"; onClicked: locale.showMessage()  }
                    MenuItem { text: "Nederlands"; onClicked: locale.showMessage()  }
                    MenuItem { text: "Polski"; onClicked: locale.showMessage()  }
                    MenuItem { text: "Русский"; onClicked: locale.showMessage()  }
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
                        settings.locale = "fa";
                        break;
                    case 4:
                        settings.locale = "nl";
                        break;
                    case 5:
                        settings.locale = "pl";
                        break;
                    case 6:
                        settings.locale = "ru";
                        break;
                    }
                }

                function showMessage() {
                    notification.show(qsTr("Changes will take effect after you restart Kaktus."));
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
                        return 4;
                    }
                }

                menu: ContextMenu {
                    MenuItem { text: qsTr("Tabs & Feeds") }
                    MenuItem { text: qsTr("Only Tabs") }
                    MenuItem { text: qsTr("All feeds") }
                    MenuItem { text: qsTr("Saved") }
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

            /*TextSwitch {
                text: qsTr("Show guide on startup")
                onCheckedChanged: {
                    settings.helpDone = !checked;
                }
                Component.onCompleted: {
                    checked = !settings.helpDone;
                }
            }*/

            Item {
                height: Theme.paddingMedium
                width: height
            }

        }

        VerticalScrollDecorator {}
    }
}
