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

    allowedOrientations: {
        switch (settings.allowedOrientations) {
        case 1:
            return Orientation.Portrait;
        case 2:
            return Orientation.Landscape;
        }
        return Orientation.Landscape | Orientation.Portrait;
    }

    SilicaListView {
        anchors { top: parent.top; left: parent.left; right: parent.right }
        clip: true

        height: {
            if (dm.busy||fetcher.busy)
                return isPortrait ? app.height-Theme.itemSizeMedium : app.width-0.8*Theme.itemSizeMedium;
            return isPortrait ? app.height : app.width;
        }

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
                                notification.show(qsTr("Signed out!"));
                                settings.signedIn = false;
                                settings.setNetvibesPassword("");
                                pageStack.clear();
                                fetcher.cancel();
                                dm.cancel();
                                db.init();
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
                enabled: false

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
            }

            /*TextSwitch {
                text: qsTr("Offline mode")
                description: qsTr("Content of items will be displayed from local cache, without a network usage.")
                onCheckedChanged: {
                    settings.offlineMode = checked;
                }
                Component.onCompleted: {
                    checked = settings.offlineMode;
                }
            }*/

            ComboBox {
                width: root.width
                label: qsTr("Mode")
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
                text: qsTr("Cache items")
                description: qsTr("After sync the content of all items will be downloaded and cached for access in Offline mode.")
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

            TextSwitch {
                text: qsTr("Show only unread items")
                onCheckedChanged: {
                    settings.showOnlyUnread = checked;
                }
                Component.onCompleted: {
                    checked = settings.showOnlyUnread;
                }
            }

            TextSwitch {
                text: qsTr("Show icons & images")
                onCheckedChanged: {
                    settings.showTabIcons = checked;
                }
                Component.onCompleted: {
                    checked = settings.showTabIcons;
                }
            }

            TextSwitch {
                text: qsTr("Show Tab with saved items")
                onCheckedChanged: {
                    settings.showStarredTab = checked;
                }
                Component.onCompleted: {
                    checked = settings.showStarredTab;
                }
            }

            TextSwitch {
                text: qsTr("Power save mode")
                description: qsTr("When the phone or app goes to the idle state, all opened web pages will be closed to lower power consumption.")
                onCheckedChanged: {
                    settings.powerSaveMode = checked;
                }
                Component.onCompleted: {
                    checked = settings.powerSaveMode;
                }
            }

            /*TextSwitch {
                text: qsTr("Auto mark as read")
                description: qsTr("All opened articles will be marked as read.")
                checked: settings.getAutoMarkAsRead();

                onCheckedChanged: {
                    settings.setAutoMarkAsRead(checked);
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

            ComboBox {
                width: root.width
                label: qsTr("Offline viewer style")
                //description: qsTr("Style which will be used to display articles in offline mode.")
                currentIndex: {
                    var theme = settings.getCsTheme();
                    if (theme === "black")
                        return 0;
                    if (theme === "white")
                        return 1;
                }
                menu: ContextMenu {
                    MenuItem { id: blackTheme; text: qsTr("Black") }
                    MenuItem { id: whiteTheme; text: qsTr("White") }
                }

                onCurrentIndexChanged: {
                    switch (currentIndex) {
                    case 0:
                        settings.setCsTheme("black");
                        break;
                    case 1:
                        settings.setCsTheme("white");
                        break;
                    }
                }
            }

        }

        VerticalScrollDecorator {}
    }
}
