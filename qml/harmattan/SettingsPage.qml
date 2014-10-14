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

    ActiveDetector {}

    ListView {
        id: listView

        anchors.fill: parent

        spacing: Theme.paddingLarge

        header: PageHeader {
            title: qsTr("Settings")
        }

        model: VisualItemModel {

            SectionHeader {
                text: qsTr("Netvibes")
            }

            TextLabel {
                text: settings.signedIn ? qsTr("Signed in as") : qsTr("Not signed in")
                value: settings.signedIn ? settings.getNetvibesUsername() : ""
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: settings.signedIn ? qsTr("Sign out") : qsTr("Sign in")
                onClicked: {
                    if (settings.signedIn) {
                        settings.signedIn = false;
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

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Delete cache")
                enabled: dm.cacheSize>0
                onClicked: {
                    fetcher.cancel(); dm.cancel();
                    dm.removeCache();
                }
            }

            ComboBox {
                label: qsTr("Network mode")
                currentIndex: settings.offlineMode ? 1 : 0

                menu: ListModel {
                    Component.onCompleted: {
                        append({text: qsTr("Online")});
                        append({text: qsTr("Offline")});
                    }
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
                label: qsTr("Language")
                currentIndex: {
                    if (settings.locale === "")
                        return 0;
                    if (settings.locale === "en")
                        return 1;
                    if (settings.locale === "fa")
                        return 2;
                    if (settings.locale === "pl")
                        return 3;
                }

                menu: ListModel {
                    Component.onCompleted: {
                        append({text: qsTr("Default")});
                        append({text: "English"});
                        append({text: "فارسی"});
                        append({text: "Polski"});
                    }
                }

                onCurrentIndexChanged: {
                    switch (currentIndex) {
                    case 0:
                        settings.locale = "";
                        break;
                    case 1:
                        settings.locale = "en";
                        break;
                    case 2:
                        settings.locale = "fa";
                        break;
                    case 3:
                        settings.locale = "pl";
                        break;
                    }
                }

                onAccepted: {
                    notification.show(qsTr("Changes will take effect after you restart Kaktus."));
                }
            }

            ComboBox {
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

                menu: ListModel {
                    Component.onCompleted: {
                        append({text: qsTr("Tabs & Feeds")});
                        append({text: qsTr("Only Tabs")});
                        append({text: qsTr("All feeds")});
                        append({text: qsTr("Saved")});
                        append({text: qsTr("Slow")});
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
                description: qsTr("When the phone or app goes to the idle state, all opened web pages will be closed to lower power consumption.")
                onCheckedChanged: {
                    settings.powerSaveMode = checked;
                }
                Component.onCompleted: {
                    checked = settings.powerSaveMode;
                }
            }

            ComboBox {
                label: qsTr("Orientation")
                currentIndex: settings.allowedOrientations

                menu: ListModel {
                    Component.onCompleted: {
                        append({text: qsTr("Dynamic")});
                        append({text: qsTr("Portrait")});
                        append({text: qsTr("Landscape")});
                    }
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
                    Component.onCompleted: {
                        append({text: qsTr("Black")});
                        append({text: qsTr("White")});
                    }
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

            SectionHeader {
                text: qsTr("Other")
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Show User Guide")
                onClicked: {
                    guide.show();
                    //notification.show(qsTr("Not yet implemented :-("));
                }
            }


            Item {
                height: Theme.paddingMedium
            }

        }
    }

    ScrollDecorator { flickableItem: listView }
}
