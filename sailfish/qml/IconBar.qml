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

    default property alias children: container.children
    property bool transparent: false

    property bool open: false
    property bool openable: true
    property bool showable: true

    property int showTime: 7000
    property real barShowMoveWidth: 20
    property Flickable flickable: null
    property bool shown: opacity == 1.0


    width: parent.width
    //height: isPortrait ? app.panelHeightPortrait : app.panelHeightLandscape
    height: Theme.itemSizeMedium
    anchors.bottom: parent.bottom
    anchors.left: parent.left
    enabled: showable
    visible: showable

    clip: true
    opacity: root.open ? 1.0 : 0.0
    Behavior on opacity { FadeAnimation {duration: 300} }

    function show() {
        if (!showable)
            return
        if (!open) {
            root.open = true;
            flick.contentX = 0;
        }
        timer.restart();
    }

    function hide() {
        if (open) {
            if (flick.dragging) {
                timer.restart();
            } else {
                root.open = false;
                timer.stop();
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        color: root.transparent ? Theme.rgba(Theme.highlightColor, 0.2) : Theme.highlightBackgroundColor
    }

    Image {
        anchors.fill: parent
        source: "image://theme/graphic-gradient-edge"
        visible: root.transparent
    }

    MouseArea {
        enabled: root.showable
        anchors { left: parent.left; right: parent.right; bottom: parent.bottom }
        height: root.open ? parent.height : parent.height / 3
        onClicked: root.show();
    }

    Item {
        height: parent.height
        width: parent.width

        // Right
        Rectangle {
            color: root.transparent ? Theme.secondaryColor : Theme.highlightDimmerColor
            height: parent.height
            width: Theme.paddingSmall
            opacity: flick.contentX < (flick.contentWidth - flick.width - Theme.paddingLarge) ? 0.5 : 0.0
            visible: opacity > 0
            anchors.right: visible ? parent.right : undefined
            Behavior on opacity {
                FadeAnimation {}
            }
        }

        // Left
        Rectangle {
            color: root.transparent ? Theme.secondaryColor : Theme.highlightDimmerColor
            height: parent.height
            width: Theme.paddingSmall
            opacity: flick.contentX > Theme.paddingLarge ? 0.5 : 0.0
            visible: opacity > 0
            anchors.left: visible ? parent.left : undefined
            Behavior on opacity {
                FadeAnimation {}
            }
        }

        Flickable {
            id: flick

            enabled: root.open
            height: parent.height
            width: parent.width
            contentWidth: container.width + 2 * Theme.paddingLarge
            contentHeight: height
            pixelAligned: true

            Flow {
                id: container
                property alias transparent: root.transparent
                property alias open: root.open
                function show() {root.show()}

                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    margins: Theme.paddingLarge
                }

                spacing: Theme.paddingLarge
            }
        }
    }

    Timer {
        id: timer
        interval: root.showTime
        onTriggered: {
            hide();
        }
    }

    QtObject {
        id: m
        property real initialContentY: 0.0
        property real lastContentY: 0.0
        property int vector: 0
    }

    Connections {
        target: root.flickable

        onMovementStarted: {
            m.vector = 0;
            //m.lastContentY = 0.0;
            m.lastContentY=root.flickable.contentY;
            m.initialContentY=root.flickable.contentY;
        }

        onContentYChanged: {
            if (root.flickable.moving) {
                var dInit = root.flickable.contentY-m.initialContentY;
                var dLast = root.flickable.contentY-m.lastContentY;
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
                    m.initialContentY=root.flickable.contentY;
                if (lastV==1 && m.vector==-1)
                    m.initialContentY=root.flickable.contentY;
                m.lastContentY = root.flickable.contentY;
            }
        }
    }
}
