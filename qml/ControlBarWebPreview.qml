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
    property bool canOffline: true
    property bool canStar: false
    property bool canOpenBrowser: false
    property bool stared: false
    property bool open: false
    property int showTime: 6000

    property real barShowMoveWidth: 20
    property Flickable flick: null

    signal backClicked()
    signal starClicked()
    signal browserClicked()

    width: parent.width
    anchors.bottom: parent.bottom
    anchors.left: parent.left

    opacity: root.open ? 1.0 : 0.0
    Behavior on opacity { FadeAnimation {duration: 300} }

    function show() {
        if (!open)
            root.open = true;
        timer.restart();
    }

    function hide() {
        if (open) {
            root.open = false;
            timer.stop();
        }
    }

    Rectangle {
        color: Theme.highlightBackgroundColor
        height: Theme.itemSizeMedium * 1
        width: parent.width


        MouseArea {
            enabled: root.opacity > 0.0
            anchors.fill: parent
            onClicked: root.hide()
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
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter

            spacing: (parent.width - (back.width * 4)) / 3;

            IconButton {
                visible: root.canStar
                icon.source: root.stared ? "image://theme/icon-m-favorite-selected?"+Theme.highlightDimmerColor : "image://theme/icon-m-favorite?"+Theme.highlightDimmerColor
                onClicked: root.starClicked()
            }

            IconButton {
                width: back.width; height: back.height
                visible: root.canOpenBrowser
                icon.source: "image://theme/icon-m-region?"+Theme.highlightDimmerColor
                onClicked: root.browserClicked()
            }

        }

        IconButton {
            id: offline
            visible: root.canOffline
            anchors.right: parent.right; anchors.rightMargin: Theme.paddingMedium
            anchors.verticalCenter: parent.verticalCenter
            icon.source: offLineMode ? "image://theme/icon-m-wlan-no-signal?"+Theme.highlightDimmerColor : "image://theme/icon-m-wlan-4?"+Theme.highlightDimmerColor
            onClicked: {
                if (offLineMode)
                    notification.show(qsTr("Switching to Online mode!"));
                else
                    notification.show(qsTr("Switching to Offline mode!"));
                offLineMode = !offLineMode;
            }
        }

        MouseArea {
            enabled: root.opacity == 0.0
            anchors.fill: parent
            onClicked: root.show();
        }
    }

    Timer {
        id: timer
        interval: root.showTime
        onTriggered: hide();
    }

    QtObject {
        id: m
        property real initialContentY: 0.0
        property real lastContentY: 0.0
        property int vector: 0
    }


    Connections {
        target: flick

        onMovementStarted: {
            m.vector = 0;
            m.lastContentY = 0.0;
            m.initialContentY=flick.contentY;
        }

        onContentYChanged: {
            if (flick.moving) {
                var dInit = flick.contentY-m.initialContentY;
                var dLast = flick.contentY-m.lastContentY;
                var lastV = m.vector;
                if (dInit<-barShowMoveWidth)
                    root.show();
                if (dInit>barShowMoveWidth)
                    root.hide();
                if (dLast>barShowMoveWidth)
                    root.hide();
                if (m.lastContentY!=0) {
                    if (dLast<0)
                        m.vector = -1;
                    if (dLast>0)
                        m.vector = 1;
                    if (dLast==0)
                        m.vector = 0;
                }
                if (lastV==-1 && m.vector==1)
                    m.initialContentY=flick.contentY;
                if (lastV==1 && m.vector==-1)
                    m.initialContentY=flick.contentY;
                m.lastContentY = flick.contentY;
            }
        }
    }

}
