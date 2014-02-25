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



Column {
    id: root

    property bool canBack: false
    property bool canStar: false
    property bool canOffline: true
    property bool stared: false
    property bool open: false
    property int showTime: 5000

    signal backClicked()
    signal starClicked()

    width: parent.width
    anchors.bottom: parent.bottom
    anchors.left: parent.left

    enabled: opacity > 0.0
    opacity: root.open ? 1.0 : 0.0
    visible: opacity > 0.0
    Behavior on opacity { FadeAnimation {duration: 300} }

    function show() {
        //timer.start();
        root.open = true;
    }

    function hide() {
        root.open = false;
    }

    Rectangle {
        color: Theme.highlightBackgroundColor
        height: Theme.itemSizeMedium * 1.2
        width: parent.width

        MouseArea {
            anchors.fill: parent
            onClicked: {}
        }

        IconButton {
            id: back
            visible: root.canBack
            anchors.left: parent.left; anchors.leftMargin: Theme.paddingMedium
            anchors.verticalCenter: parent.verticalCenter
            icon.source: "image://theme/icon-m-back?"+Theme.highlightDimmerColor
            onClicked: root.backClicked()
        }

        Row {
            id: toolbarRow

            anchors {
                left: back.right; leftMargin: Theme.paddingMedium
                right: offline.left; rightMargin: Theme.paddingMedium
                verticalCenter: parent.verticalCenter
            }

            spacing: (width - (back.width * 3)) / 2;

            /*spacing: {
                var i = 0;
                if (canBack)
                    ++i;
                if (canStar)
                    ++i;
                if (canOffline)
                    ++i;
                return (width - (back.width * i)) / i-1;
            }*/

            IconButton {
                visible: root.canStar
                icon.source: root.stared ? "image://theme/icon-m-favorite-selected?"+Theme.highlightDimmerColor : "image://theme/icon-m-favorite?"+Theme.highlightDimmerColor
                onClicked: root.starClicked()
            }

        }

        IconButton {
            id: offline
            visible: root.canOffline
            anchors.right: parent.right; anchors.rightMargin: Theme.paddingMedium
            anchors.verticalCenter: parent.verticalCenter
            icon.source: offLineMode ? "image://theme/icon-m-wlan-no-signal?"+Theme.highlightDimmerColor : "image://theme/icon-m-wlan-4?"+Theme.highlightDimmerColor
            onClicked: {
                offLineMode = !offLineMode;
                if (offLineMode)
                    notification.show(qsTr("Offline mode enabled"));
                else
                    notification.show(qsTr("Online mode enabled"));
            }
        }
    }

    Timer {
        id: timer
        interval: root.showTime
        onTriggered: hide();
    }
}
