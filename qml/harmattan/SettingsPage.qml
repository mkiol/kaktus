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

import QtQuick 1.1
import com.nokia.meego 1.0

import "Theme.js" as Theme

Page {
    id: root

    tools: SimpleToolbar {}

    orientationLock: {
        switch (settings.allowedOrientations) {
        case 1:
            return PageOrientation.LockPortrait;
        case 2:
            return PageOrientation.LockLandscape;
        }
        return PageOrientation.Automatic;
    }

    PageHeader {
        id: header
        title: qsTr("Settings")
    }

    ListView {
        id: listView

        anchors {
            top: header.bottom; topMargin: Theme.paddingMedium
            left: parent.left; right: parent.right;
            bottom: parent.bottom; bottomMargin: Theme.paddingMedium
        }

        spacing: Theme.paddingLarge

        model: VisualItemModel {

            TextLabel {
                text: settings.signedIn ? qsTr("Signed in as") : qsTr("Not signed in")
                value: settings.signedIn ? settings.getNetvibesUsername() : ""
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: settings.signedIn ? qsTr("Sign out") : qsTr("Sign in")
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

            TextButtonLabel {
                text: settings.signedIn && utils.defaultDashboardName()!=="" ? qsTr("Dashboard in use") : qsTr("Dashboard not selected")
                value: utils.defaultDashboardName()

                onClicked: {
                    utils.setDashboardModel();
                    pageStack.push(Qt.resolvedUrl("DashboardDialog.qml"));
                }
            }

            SectionHeader {
                text: qsTr("Cache")
            }

            TextLabel {
                text: qsTr("Current cache size")
                value: utils.getHumanFriendlySizeString(dm.cacheSize);
            }

            TextSwitch {
                text: qsTr("Offline mode")
                description: qsTr("Content of items will be displayed from local cache, without a network usage.")
                onCheckedChanged: {
                    settings.offlineMode = checked;
                }
                Component.onCompleted: {
                    checked = settings.offlineMode;
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
                text: qsTr("Show icons")
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
                label: qsTr("Orientation")
                currentIndex: settings.allowedOrientations

                menu: ListModel {
                    ListElement { text: "Dynamic" }
                    ListElement { text: "Portrait" }
                    ListElement { text: "Landscape" }
                }

                onCurrentIndexChanged: {
                    settings.allowedOrientations = currentIndex;
                }
            }

            ComboBox {
                label: qsTr("Offline viewer style")
                currentIndex: {
                    var theme = settings.getCsTheme();
                    if (theme === "black")
                        return 0;
                    if (theme === "white")
                        return 1;
                }

                menu: ListModel {
                    ListElement { text: "Black" }
                    ListElement { text: "White" }
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
    }

    ScrollDecorator { flickableItem: listView }
}
