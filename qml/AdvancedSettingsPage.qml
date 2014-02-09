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
            title: qsTr("Advanced Settings")
        }

        Component.onDestruction: {
            // Save settings
            settings.setDmUserAgent(useragent.text);
        }

        model: VisualItemModel {

            TextArea {
                id: useragent
                anchors.left: parent.left;
                anchors.right: parent.right;
                placeholderText: qsTr("User Agent string")
                label: qsTr("User Agent string")
                Component.onCompleted: {
                    text = settings.getDmUserAgent();
                }
            }

            /*
            // General

            Label {
                text: qsTr("Settings directory")
            }
            TextField {
                id: settingsdir
                Component.onCompleted: {
                    text = settings.getSettingsDir();
                }
            }

            // Netvibes Fetcher

            Label {
                text: qsTr("Default Dashboard")
            }
            TextField {
                id: dashboard
                Component.onCompleted: {
                    text = settings.getNetvibesDefaultDashboard();
                }
            }

            Label {
                text: qsTr("Feed Limit")
            }
            TextField {
                id: feedlimit
                Component.onCompleted: {
                    text = settings.getNetvibesFeedLimit();
                }
            }

            Label {
                text: qsTr("Feed update at once")
            }
            TextField {
                id: feedupdateatonce
                Component.onCompleted: {
                    text = settings.getNetvibesFeedUpdateAtOnce();
                }
            }

            // Download Manger

            Label {
                text: qsTr("Connections")
            }
            TextField {
                id: connections
                Component.onCompleted: {
                    text = settings.getDmConnections();
                }
            }

            Label {
                text: qsTr("Timeout")
            }
            TextField {
                id: timeout
                Component.onCompleted: {
                    text = settings.getDmTimeOut();
                }
            }

            Label {
                text: qsTr("Maximum size of file")
            }
            TextField {
                id: maxfile
                Component.onCompleted: {
                    text = settings.getDmMaxSize();
                }
            }

            Label {
                text: qsTr("Cache directory")
            }
            TextField {
                id: cachedir
                Component.onCompleted: {
                    text = settings.getDmCacheDir();
                }
            }

            Label {
                text: qsTr("User Agent string")
            }
            TextField {
                id: useragent
                Component.onCompleted: {
                    text = settings.getDmUserAgent();
                }
            }

            Label {
                text: qsTr("Maximum retency time")
            }
            TextField {
                id: maxretency
                Component.onCompleted: {
                    text = settings.getDmMaxCacheRetency();
                }
            }

            Label {
                text: qsTr("Retency feed limit")
            }
            TextField {
                id: retencyfeedlimit
                Component.onCompleted: {
                    text = settings.getDmCacheRetencyFeedLimit();
                }
            }
            */
        }

        VerticalScrollDecorator {}
    }
}
