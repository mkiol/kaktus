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



Item {
    id: root

    property bool open: true
    property int showTime: 8000
    property real barShowMoveWidth: 20
    property Flickable flick: null

    width: parent.width
    anchors.bottom: parent.bottom
    anchors.left: parent.left
    height: Theme.itemSizeMedium * 1

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
        id: background
        color: Theme.rgba(Theme.backgroundColor, 0.7)
        anchors.fill: parent
    }

    OpacityRampEffect {
        id: effect
        slope: 2
        offset: 0.5
        direction: OpacityRamp.RightToLeft
        sourceItem: background
    }

    //color: Theme.rgba(Theme.highlightBackgroundColor, 0.3)
    //color: Theme.rgba(Theme.backgroundColor, 0.7)
    //height: Theme.itemSizeMedium * 1
    //width: parent.width
    //anchors.fill: parent

    MouseArea {
        enabled: root.opacity > 0.0
        anchors.fill: parent
        onClicked: root.hide()
    }

    Row {
        id: row
        anchors.right: parent.right; anchors.rightMargin: Theme.paddingMedium
        anchors.verticalCenter: parent.verticalCenter

        Label {
            id: label
            anchors.verticalCenter: parent.verticalCenter
            text: offLineMode ? qsTr("Offline mode") : qsTr("Online mode")
            font.pixelSize: Theme.fontSizeSmall
            font.family: Theme.fontFamily
            color: Theme.highlightColor
            opacity: 0.0
            visible: opacity > 0.0
            Behavior on opacity { FadeAnimation {duration: 200} }

            MouseArea {
                anchors.fill: parent
                onClicked: offLineMode = !offLineMode
            }

            function show() {
                label.opacity = 1.0;
                ltimer.restart();
            }

            Timer {
                id: ltimer
                interval: 3000
                onTriggered: {
                    label.opacity = 0.0;
                }
            }
        }

        IconButton {
            id: offline
            anchors.verticalCenter: parent.verticalCenter
            icon.source: offLineMode ? "image://theme/icon-m-wlan-no-signal?"+Theme.highlightColor : "image://theme/icon-m-wlan-4?"+Theme.highlightColor
            onClicked: {
                offLineMode = !offLineMode;
                label.show();
            }
        }
    }

    MouseArea {
        enabled: root.opacity == 0.0
        anchors.fill: parent
        onClicked: root.show();
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
