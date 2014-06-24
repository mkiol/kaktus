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
import com.nokia.symbian 1.1

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
        title: qsTr("About")
        opacity: 0.0
    }

    ListView {
        id: listView

        anchors {
            top: header.bottom; topMargin: platformStyle.paddingMedium
            left: parent.left; right: parent.right;
            bottom: parent.bottom; bottomMargin: platformStyle.paddingMedium
        }

        spacing: 1.5*platformStyle.paddingLarge

        model: VisualItemModel {

            Image {
                anchors.horizontalCenter: parent.horizontalCenter
                source: "icon.png"
            }

            Label {
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    leftMargin: platformStyle.paddingMedium; rightMargin: platformStyle.paddingMedium
                }

                font.pixelSize: 1.5*platformStyle.fontSizeLarge
                text: APP_NAME
            }

            Label {
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    leftMargin: platformStyle.paddingMedium; rightMargin: platformStyle.paddingMedium
                }
                font.pixelSize: platformStyle.fontSizeMedium
                color: platformStyle.colorNormalLight
                wrapMode: Text.WordWrap
                text: qsTr("Version: %1").arg(VERSION);
            }

            Label {
                anchors {
                    leftMargin: platformStyle.paddingMedium; rightMargin: platformStyle.paddingMedium
                    left: parent.left; right: parent.right
                }
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: platformStyle.fontSizeSmall
                text: qsTr("An unofficial Netvibes feed reader, specially designed to work offline.");
            }

            Label {
                anchors {
                    leftMargin: platformStyle.paddingMedium; rightMargin: platformStyle.paddingMedium
                    left: parent.left; right: parent.right
                }
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: platformStyle.fontSizeSmall
                textFormat: Text.StyledText
                text: PAGE;
            }

            Label {
                anchors {
                    leftMargin: platformStyle.paddingMedium; rightMargin: platformStyle.paddingMedium
                    topMargin: platformStyle.paddingLarge
                    left: parent.left; right: parent.right
                }
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: platformStyle.fontSizeSmall
                textFormat: Text.RichText
                text: "Copyright &copy; 2014 Michał Kościesza"
            }
        }
    }

    ScrollDecorator { flickableItem: listView }
}
