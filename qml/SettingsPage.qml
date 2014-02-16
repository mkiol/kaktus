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
                        text: qsTr("Not signed in into Netvibes")
                        visible: !signinForm.signedIn
                    }
                    Label {
                        text: qsTr("Signed in into Netvibes as:")
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
                        text: signinForm.signedIn ? qsTr("Sign Out") : qsTr("Sign In")
                        onClicked: {
                            if (signinForm.signedIn) {
                                settings.setSignedIn(false);
                                settings.setNetvibesPassword("");
                                signinForm.signedIn = false;
                            } else {
                                pageStack.push(Qt.resolvedUrl("SignInDialog.qml"),{"code": 0});
                            }
                        }
                    }
                }

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

                Flow {
                    id: flow2
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left; anchors.right: parent.right
                    spacing: Theme.paddingMedium
                    anchors.leftMargin: Theme.paddingLarge
                    anchors.rightMargin: Theme.paddingLarge

                    Label {
                        text: qsTr("Dashboard in use: ")
                    }

                    Label {
                        id: dashboard
                        color: Theme.highlightColor
                        text: settings.getNetvibesDefaultDashboard()
                    }
                }

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

            TextSwitch {
                id: autodownload
                text: qsTr("Caching articles")
                description: qsTr("If enabled, all articles will be downloaded and cached for access in Offline mode after every Sync action.")

                Component.onCompleted: {
                    checked = settings.getAutoDownloadOnUpdate();
                }

                onCheckedChanged: {
                    settings.setAutoDownloadOnUpdate(checked);
                }
            }

            TextSwitch {
                id: offlinemode
                text: qsTr("Offline mode")
                description: qsTr("If enabled, content of articles will be displayed from local cache, without a network usage.")

                Component.onCompleted: {
                    checked = settings.getOfflineMode();
                }

                onCheckedChanged: {
                    settings.setOfflineMode(checked);
                }
            }

            /*ComboBox {
                id: defaultdashboard
                width: root.width
                label: qsTr("Default Dashboard")

                Component.onCompleted: {
                    var menustr = 'import QtQuick 2.0;import Sailfish.Silica 1.0;ContextMenu{';
                    var list = utils.dashboards();
                    for (var i in list)
                        menustr += 'MenuItem{text:"'+list[i]+'"}';
                    menustr += '}';
                    menu = Qt.createQmlObject(menustr,defaultdashboard);
                    defaultdashboard.currentIndex = 0;
                }
            }*/

            /*Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Advanced Settings")
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("AdvancedSettingsPage.qml"));
                }
            }*/

        }

        VerticalScrollDecorator {}
    }
}
