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

    property bool open: false
    property int showTime: 6000
    property bool transparent: true

    property real barShowMoveWidth: 20
    property Flickable flick: null

    property bool isPortrait: app.orientation==Orientation.Portrait

    height: Theme.itemSizeMedium
    width: parent.width

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

    /*Rectangle {
        anchors.fill: parent
        visible: root.transparent
        color: Theme.rgba(Theme.highlightColor, 0.2)
        gradient: Gradient {
            GradientStop { position: 0.0; color: Theme.rgba(Theme.highlightDimmerColor, 0.0) }
            GradientStop { position: 1.0; color: Theme.rgba(Theme.highlightDimmerColor, 0.5) }
        }
    }*/

    Item {
        id: bar
        anchors.fill: parent

        opacity: root.open ? 1.0 : 0.0
        visible: opacity > 0.0
        Behavior on opacity { FadeAnimation {duration: 300} }

        Rectangle {
            anchors.fill: parent
            visible: !root.transparent
            color: Theme.highlightBackgroundColor
        }

        /*Image {
            anchors.left: parent.left; anchors.right: parent.right
            source: "image://theme/graphic-gradient-home-top?"+Theme.highlightBackgroundColor
            visible: root.transparent
            opacity: 0.3
        }*/

        MouseArea {
            enabled: bar.visible
            anchors.fill: parent
            onClicked: root.hide()
        }

        Row {
            id: toolbarRow
            anchors.verticalCenter: parent.verticalCenter
            //anchors.horizontalCenter: parent.horizontalCenter
            anchors.left: parent.left; anchors.right: offline.left;
            anchors.leftMargin: Theme.paddingMedium
            spacing: Theme.paddingSmall


            IconButton {
              icon.source: "image://icons/vm0?"+(root.transparent ? Theme.primaryColor : Theme.highlightDimmerColor)
              highlighted: settings.viewMode==0
              onClicked: {
                    settings.viewMode = 0;
                    show();
              }
            }

            IconButton {
                icon.source: "image://icons/vm1?"+(root.transparent ? Theme.primaryColor : Theme.highlightDimmerColor)
                highlighted: settings.viewMode==1
                onClicked: {
                    settings.viewMode = 1;
                    show();
                }
            }

            /*IconButton {
                icon.source: "image://icons/vm2?"+(root.transparent ? Theme.primaryColor : Theme.highlightDimmerColor)
                highlighted: settings.viewMode==2
                onClicked: {
                    settings.viewMode = 2;
                    timer.restart();
                }
            }*/

            IconButton {
                icon.source: "image://icons/vm3?"+(root.transparent ? Theme.primaryColor : Theme.highlightDimmerColor)
                highlighted: settings.viewMode==3
                onClicked: {
                    settings.viewMode = 3;
                    show();
                }
            }

            IconButton {
                icon.source: "image://icons/vm4?"+(root.transparent ? Theme.primaryColor : Theme.highlightDimmerColor)
                highlighted: settings.viewMode==4
                onClicked: {
                    settings.viewMode = 4;
                    show();
                }
            }

            IconButton {
                icon.source: "image://icons/vm5?"+(root.transparent ? Theme.primaryColor : Theme.highlightDimmerColor)
                highlighted: settings.viewMode==5
                onClicked: {
                    //settings.viewMode = 5;
                    show();
                }
            }

        }

        IconButton {
            id: offline
            anchors.right: parent.right; anchors.rightMargin: Theme.paddingMedium
            anchors.verticalCenter: parent.verticalCenter
            icon.source: settings.offlineMode ? "image://theme/icon-m-wlan-no-signal?"+(root.transparent ? Theme.primaryColor : Theme.highlightDimmerColor)
                                              : "image://theme/icon-m-wlan-4?"+(root.transparent ? Theme.primaryColor : Theme.highlightDimmerColor)
            onClicked: {
                show();
                if (settings.offlineMode) {
                    if (dm.online)
                        settings.offlineMode = false;
                    else
                        notification.show(qsTr("Cannot switch to Online mode\nNetwork connection is unavailable"));
                } else {
                    settings.offlineMode = true;
                }
            }
        }
    }

    MouseArea {
        enabled: !bar.visible
        anchors.bottom: parent.bottom; anchors.left: parent.left; anchors.right: parent.right
        height: root.height/3
        onClicked: root.show();
    }

    Timer {
        id: timer
        interval: root.showTime
        //onTriggered: hide();
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
