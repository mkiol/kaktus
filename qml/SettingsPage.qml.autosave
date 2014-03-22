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

    SilicaListView {
        anchors.fill: parent

        header: PageHeader {
            title: qsTr("Settings")
        }

        model: VisualItemModel {

            SectionHeader {
                text: qsTr("Netvibes account")
            }

            ListItem {
                id: signinForm

                property bool signedIn: settings.getSignedIn()
                onSignedInChanged: usernameLabel.text = settings.getNetvibesUsername();
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
                        visible: !signinForm.signedIn
                    }
                    Label {
                        text: qsTr("Signed in as")
                        visible: signinForm.signedIn
                    }
                    Label {
                        id: usernameLabel
                        color: Theme.highlightColor
                        visible: signinForm.signedIn
                    }
                }

                menu: ContextMenu {
                    MenuItem {
                        text: signinForm.signedIn ? qsTr("Sign out") : qsTr("Sign in")
                        onClicked: {
                            if (signinForm.signedIn) {
                                notification.show(qsTr("Signed out!"));
                                settings.setSignedIn(false);
                                settings.setNetvibesPassword("");
                                signinForm.signedIn = false;
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

                Connections {
                    target: fetcher
                    onCredentialsValid: {
                        signinForm.signedIn = settings.getSignedIn();
                    }
                }
            }

            ListItem {
                id: defaultdashboard
                contentHeight: flow2.height + 2*Theme.paddingLarge
                enabled: signinForm.signedIn && utils.defaultDashboardName()!==""
                Flow {
                    id: flow2
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left; anchors.right: parent.right
                    spacing: Theme.paddingMedium
                    anchors.leftMargin: Theme.paddingLarge
                    anchors.rightMargin: Theme.paddingLarge

                    Label {
                        color: signinForm.signedIn && utils.defaultDashboardName()!=="" ? Theme.primaryColor : Theme.secondaryColor
                        text: signinForm.signedIn && utils.defaultDashboardName()!=="" ? qsTr("Dashboard in use") : qsTr("Dashboard not selected")
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
                text: qsTr("Other")
            }

            TextSwitch {
                text: qsTr("Offline mode")
                description: qsTr("Content of articles will be displayed from local cache, without a network usage.")

                checked: settings.getOfflineMode()

                onCheckedChanged: {
                    settings.setOfflineMode(checked);
                }
            }

            TextSwitch {
                text: qsTr("Cache articles")
                description: qsTr("After sync the content of all articles will be downloaded and cached for access in Offline mode.")

                Component.onCompleted: {
                    checked = settings.getAutoDownloadOnUpdate();
                }

                onCheckedChanged: {
                    settings.setAutoDownloadOnUpdate(checked);
                }
            }

            TextSwitch {
                text: qsTr("Show icons")
                checked: settings.getShowTabIcons();

                onCheckedChanged: {
                    settings.setShowTabIcons(checked);
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

            /*TextSwitch {
                text: qsTr("Show Tabs icons")
                checked: settings.getShowTabIcons();

                onCheckedChanged: {
                    settings.setShowTabIcons(checked);
                }
            }*/

        }

        VerticalScrollDecorator {}
    }
}
