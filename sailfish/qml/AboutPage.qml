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


        anchors.leftMargin: Theme.paddingLarge
        anchors.rightMargin: Theme.paddingLarge
        spacing: Theme.paddingLarge

        header: PageHeader {
            title: qsTr("About")
        }

        model: VisualItemModel {

            Image {
                anchors.horizontalCenter: parent.horizontalCenter
                source: "image://icons/icon-i-kaktus"
            }

            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: Theme.fontSizeHuge
                text: APP_NAME
            }

            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: Theme.fontSizeMedium
                color: Theme.highlightColor
                wrapMode: Text.WordWrap
                text: qsTr("Version: %1").arg(VERSION);
            }

            Label {
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                anchors.left: parent.left; anchors.right: parent.right
                font.pixelSize: Theme.fontSizeSmall
                text: qsTr("Multi services feed reader, specially designed to work offline.");
            }

            /*Separator {
                anchors.left: parent.left
                anchors.right: parent.right
                color: Theme.primaryColor
            }*/

            Label {
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                anchors.left: parent.left; anchors.right: parent.right
                font.pixelSize: Theme.fontSizeExtraSmall
                text: "<u>%1</u>".arg(PAGE)
                textFormat: Text.StyledText

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        notification.show(qsTr("Launching an external browser..."));
                        Qt.openUrlExternally(PAGE);
                    }
                }
            }

            /*Separator {
                anchors.left: parent.left
                anchors.right: parent.right
                color: Theme.primaryColor
            }*/

            Label {
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                anchors.left: parent.left; anchors.right: parent.right
                font.pixelSize: Theme.fontSizeExtraSmall
                textFormat: Text.RichText
                text: "Copyright &copy; 2014-2017 Michal Kosciesza"
            }

            Label {
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                anchors.left: parent.left; anchors.right: parent.right
                font.pixelSize: Theme.fontSizeExtraSmall
                text: qsTr("This software is distributed under the terms of the "+
                           "GNU General Public Licence version 3.")
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        notification.show(qsTr("Launching an external browser..."));
                        Qt.openUrlExternally("https://www.gnu.org/licenses/gpl-3.0.txt");
                    }
                }

            }

            /*Label {
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.HorizontalFit
                anchors.left: parent.left; anchors.right: parent.right

                font.pixelSize: Theme.fontSizeExtraSmall
                text: qsTr("Be aware that Kaktus is an UNOFFICIAL application. It means is distributed in the hope " +
                           "that it will be useful, but WITHOUT ANY WARRANTY. Without even the implied warranty of " +
                           "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. " +
                           "See the GNU General Public License for more details.")

            }*/

            /*Separator {
                anchors.left: parent.left
                anchors.right: parent.right
                color: Theme.primaryColor
            }*/

            Item {
                height: Theme.paddingLarge
            }

            Button {
                text: qsTr("Changelog")
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: pageStack.push(Qt.resolvedUrl("ChangelogPage.qml"))
            }

            Item {
                height: Theme.paddingLarge
            }
        }

        //VerticalScrollDecorator {}
    }
}
