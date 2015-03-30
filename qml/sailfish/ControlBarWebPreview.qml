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

    property bool canBack: false
    property bool canReader: false
    property bool canStar: false
    property bool canOpenBrowser: false
    property bool canFontUp: false
    property bool canFontDown: false
    property bool canClipboard: false
    property bool stared: false
    property bool open: false
    property int showTime: 6000
    property bool transparent: true

    property real barShowMoveWidth: 20
    property Flickable flick: null

    property bool isPortrait: app.orientation==Orientation.Portrait

    function getSpacing() {
        var count = 0;
        if (canBack)
            count++;
        if (canReader)
            count++;
        if (canStar)
            count++;
        if (canOpenBrowser)
            count++;
        if (canFontUp)
            count++;
        if (canFontDown)
            count++;
        if (canClipboard)
            count++;
        return (root.width - (back.width * (count))) / count-1;
    }

    signal backClicked()
    signal starClicked()
    signal browserClicked()
    signal offlineClicked()
    signal readerClicked()
    signal fontUpClicked()
    signal fontDownClicked()
    signal clipboardClicked()

    width: parent.width
    height: isPortrait ? app.panelHeightPortrait : app.panelHeightLandscape
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
        anchors.fill: parent
        visible: root.transparent
        color: Theme.rgba(Theme.highlightColor, 0.2)
    }

    Rectangle {
        anchors.fill: parent
        visible: !root.transparent
        color: Theme.highlightBackgroundColor
    }

    Image {
        anchors.fill: parent
        //fillMode: Image.PreserveAspectFit
        source: "image://theme/graphic-gradient-home-top"
        visible: root.transparent
    }

    Item {
        anchors.fill: parent

        MouseArea {
            enabled: root.opacity > 0.0
            anchors.fill: parent
            onClicked: root.hide()
        }

        /*IconButton {
            id: back
            visible: root.canBack
            anchors.left: parent.left; anchors.leftMargin: Theme.paddingMedium
            anchors.verticalCenter: parent.verticalCenter
            icon.source: "image://theme/icon-m-back?"+(root.transparent ? Theme.highlightColor : Theme.highlightDimmerColor)
            onClicked: root.backClicked()
        }*/

        Row {
            id: toolbarRow
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter

            spacing: root.getSpacing()

            IconButton {
                id: back
                visible: root.canBack
                icon.source: "image://theme/icon-m-back?"+(root.transparent ? Theme.highlightColor : Theme.highlightDimmerColor)
                onClicked: {root.backClicked();show();}
            }

            IconButton {
                visible: root.canStar
                icon.source: root.stared ?
                                 "image://theme/icon-m-favorite-selected?"+(root.transparent ? Theme.highlightColor : Theme.highlightDimmerColor)
                               : "image://theme/icon-m-favorite?"+(root.transparent ? Theme.highlightColor : Theme.highlightDimmerColor)
                onClicked: {root.starClicked();show();}
            }

            IconButton {
                width: back.width; height: back.height
                visible: root.canClipboard
                icon.source: "image://theme/icon-m-clipboard?"+(root.transparent ? Theme.highlightColor : Theme.highlightDimmerColor)
                onClicked: {root.clipboardClicked();show();}
            }

            IconButton {
                width: back.width; height: back.height
                visible: root.canOpenBrowser
                icon.source: "image://theme/icon-m-region?"+(root.transparent ? Theme.highlightColor : Theme.highlightDimmerColor)
                onClicked: {root.browserClicked();show();}
            }

            IconButton {
                width: back.width; height: back.height
                visible: root.canFontUp
                icon.source: "image://icons/fontup?"+(root.transparent ? Theme.highlightColor : Theme.highlightDimmerColor)
                onClicked: {root.fontUpClicked();show();}
            }

            IconButton {
                width: back.width; height: back.height
                visible: root.canFontDown
                icon.source: "image://icons/fontdown?"+(root.transparent ? Theme.highlightColor : Theme.highlightDimmerColor)
                onClicked: {root.fontDownClicked();show();}
            }

            IconButton {
                visible: root.canReader
                width: back.width; height: back.height
                /*icon.source: settings.readerMode ? "image://theme/icon-m-document?"+(root.transparent ? Theme.highlightColor : Theme.highlightDimmerColor)
                                                 : "image://theme/icon-m-document?"+(root.transparent ? Qt.darker(Theme.highlightColor,3.5) : Qt.lighter(Theme.highlightDimmerColor,3.5))
                */
                icon.source: settings.readerMode ? "image://icons/reader?"+(root.transparent ? Theme.highlightColor : Theme.highlightDimmerColor)
                                                 : "image://icons/reader-disabled?"+(root.transparent ? Theme.highlightColor : Theme.highlightDimmerColor)

                onClicked: {
                    settings.readerMode = !settings.readerMode;
                    root.readerClicked();
                    show();
                }
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
