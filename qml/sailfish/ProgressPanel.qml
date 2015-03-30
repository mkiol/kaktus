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

    property string text
    property bool cancelable: true
    property bool open: false
    property real progress: 0.0
    property bool transparent: true
    signal closeClicked

    enabled: open
    opacity: open ? 1.0 : 0.0
    visible: opacity > 0.0

    height: Theme.itemSizeSmall
    width: parent.width

    Behavior on opacity { FadeAnimation {} }

    /*Rectangle {
        anchors.fill: parent
        visible: root.transparent
	
        color: Theme.rgba(Theme.highlightColor, 0.2)
    }*/

    Rectangle {
        anchors.fill: parent
        visible: !root.transparent
        color: Theme.highlightBackgroundColor
    }

    function show(text) {
        root.text = text;
        root.open = true;
    }

    function hide() {
        root.open = false;
        root.progress = 0.0;
    }

    Image {
        anchors.left: parent.left; anchors.right: parent.right
        //anchors.fill: parent
        //fillMode: Image.PreserveAspectFit
        source: "image://theme/graphic-gradient-home-top?"+Theme.highlightBackgroundColor
        //source: "image://theme/graphic-gradient-home-bottom?"+Theme.highlightBackgroundColor
        //source: "image://theme/graphic-gradient-edge?"+Theme.highlightBackgroundColor
        //source: "image://theme/graphic-keyboard-highlight-top?"+Theme.highlightBackgroundColor
        visible: root.transparent
        opacity: 0.8
    }

    Rectangle {
        id: progressRect
        height: parent.height
        anchors.left: parent.left
        width: root.progress * parent.width
        color: root.transparent ? Theme.rgba(Theme.highlightBackgroundColor, 0.3) : Theme.rgba(Theme.highlightDimmerColor, 0.2)

        Behavior on width {
            //enabled: root.opacity == 1.0
            SmoothedAnimation {
                velocity: 480; duration: 200
            }
        }
    }

    Image {
        id: icon
        height: 60; width: 60
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: Theme.paddingMedium
        source: "image://theme/graphic-busyindicator-medium?"+(root.transparent ? Theme.highlightColor : Theme.highlightDimmerColor)
        RotationAnimation on rotation {
            loops: Animation.Infinite
            from: 0
            to: 360
            duration: 1200
            running: root.open && Qt.application.active
        }
    }

    onVisibleChanged: {
        if (!visible) {
            progress = 0;
        }
    }

    Label {
        id: titleBar
        height: icon.height
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: icon.right; anchors.right: closeButton.left
        anchors.leftMargin: Theme.paddingMedium; anchors.rightMargin: Theme.paddingMedium
        font.pixelSize: Theme.fontSizeSmall
        font.family: Theme.fontFamily
        text: root.text
        color: root.transparent ? Theme.highlightColor : Theme.highlightDimmerColor
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
    }

    IconButton {
        id: closeButton
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        icon.source: "image://theme/icon-m-close?"+(root.transparent ? Theme.highlightColor : Theme.highlightDimmerColor)
        onClicked: root.closeClicked()
        visible: root.cancelable
    }


}
