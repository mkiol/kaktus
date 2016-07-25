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
    property bool openable: true
    property int showTime: 7000

    property real barShowMoveWidth: 20
    property real barShowMoveWidthBack: height
    property Flickable flick: null

    property bool isPortrait: app.orientation==Orientation.Portrait

    height: Theme.itemSizeMedium
    width: parent.width

    clip: true

    function show() {
        if (openable && pageStack.currentPage.showBar) {
            if (!open)
                root.open = true;
            timer.restart();
        }
    }

    function showAndEnable() {
        openable = true
        show();
    }

    function hide() {
        if (open) {
            root.open = false;
            timer.stop();
        }
    }

    function hideAndDisable() {
        hide();
        openable = false;
    }

    Item {
        id: bar
        anchors.fill: parent

        opacity: root.open ? 1.0 : 0.0
        visible: opacity > 0.0
        Behavior on opacity { FadeAnimation {duration: 300} }

        property int off: 5
        property int size: vm0b.width + Theme.paddingMedium

        Rectangle {
            id: leftRect
            anchors.left: parent.left
            anchors.top: parent.top; anchors.bottom: parent.bottom
            color: Theme.highlightBackgroundColor
            width: settings.viewMode==0 ? 0 :
                   settings.viewMode==1 ? 1 * bar.size - bar.off :
                   settings.viewMode==3 ? 2 * bar.size - bar.off :
                   settings.viewMode==4 ? 3 * bar.size - bar.off :
                   settings.viewMode==5 ? 4 * bar.size - bar.off :
                   settings.viewMode==6 ? 4 * bar.size - bar.off :
                   5 * bar.size
            Behavior on width { NumberAnimation { duration: 200;easing.type: Easing.OutQuad } }
        }

        Rectangle {
            id: rightRect
            anchors.right: parent.right
            anchors.top: parent.top; anchors.bottom: parent.bottom
            color: Theme.highlightBackgroundColor
            width: settings.viewMode==0 ? root.width - bar.size + bar.off :
                   settings.viewMode==1 ? root.width - 2 * bar.size + bar.off :
                   settings.viewMode==3 ? root.width - 3 * bar.size + bar.off :
                   settings.viewMode==4 ? root.width - 4 * bar.size + bar.off :
                   settings.viewMode==5 ? root.width - 5 * bar.size + bar.off :
                   settings.viewMode==6 ? root.width - 5 * bar.size + bar.off :
                   0
            Behavior on width { NumberAnimation { duration: 200;easing.type: Easing.OutQuad } }
        }

        Rectangle {
            anchors.right: rightRect.left; anchors.left: leftRect.right
            anchors.bottom: parent.bottom;
            height: root.height/10
            color: Theme.highlightColor
        }

        MouseArea {
            enabled: bar.visible
            anchors.fill: parent
            onClicked: root.hide()
        }

        Item {
            property int off: -root.height/10
            anchors.left: parent.left; anchors.right: parent.right
            anchors.bottom: parent.bottom; height: parent.height

            IconButton {
                id: vm0b
                x: 0*(width+Theme.paddingMedium)
                y: ((parent.height-height)/2) + (highlighted ? parent.off : 0)
                icon.source: "image://icons/icon-m-vm0?"+Theme.highlightDimmerColor
                highlighted: settings.viewMode==0
                onClicked: {
                    show();
                    if (!app.progress && settings.viewMode!=0) {
                        app.progress = true;
                        settings.viewMode = 0;
                    }
                }
            }

            IconButton {
                id: vm1b
                x: 1*(width+Theme.paddingMedium)
                y: ((parent.height-height)/2) + (highlighted ? parent.off : 0)
                icon.source: "image://icons/icon-m-vm1?"+Theme.highlightDimmerColor
                highlighted: settings.viewMode==1
                onClicked: {
                    show();
                    if (!app.progress && settings.viewMode!=1) {
                        app.progress = true;
                        settings.viewMode = 1;
                    }
                }
            }

            IconButton {
                id: vm3b
                x: 2*(width+Theme.paddingMedium)
                y: ((parent.height-height)/2) + (highlighted ? parent.off : 0)
                icon.source: "image://icons/icon-m-vm3?"+Theme.highlightDimmerColor
                highlighted: settings.viewMode==3
                onClicked: {
                    show();
                    if (!app.progress && settings.viewMode!=3) {
                        app.progress = true;
                        settings.viewMode = 3;
                    }
                }
            }

            IconButton {
                id: vm4b
                x: 3*(width+Theme.paddingMedium)
                y: ((parent.height-height)/2) + (highlighted ? parent.off : 0)
                icon.source: "image://icons/icon-m-vm4?"+Theme.highlightDimmerColor
                highlighted: settings.viewMode==4
                onClicked: {
                    show();
                    if (!app.progress && settings.viewMode!=4) {
                        app.progress = true;
                        settings.viewMode = 4;
                    }
                }
            }

            IconButton {
                id: vm5b
                x: 4*(width+Theme.paddingMedium)
                visible: app.isNetvibes || (app.isOldReader && settings.showBroadcast) // Disabled for Feedly
                y: ((parent.height-height)/2) + (highlighted ? parent.off : 0)
                icon.source: app.isOldReader ? "image://icons/icon-m-vm6?"+(root.transparent ? Theme.primaryColor : Theme.highlightDimmerColor) :
                                               "image://icons/icon-m-vm5?"+(root.transparent ? Theme.primaryColor : Theme.highlightDimmerColor)
                highlighted: settings.viewMode==5 || settings.viewMode==6
                onClicked: {
                    show();
                    if (!app.progress && (settings.viewMode!=5 || settings.viewMode!=6)) {
                        app.progress = true;
                        if (settings.signinType >= 10)
                            settings.viewMode = 6;
                        else
                            settings.viewMode = 5;
                    }
                }
            }

            IconButton {
                visible: pageStack.currentPage.objectName != ""
                anchors.right: parent.right; anchors.rightMargin: Theme.paddingSmall
                anchors.verticalCenter: parent.verticalCenter
                icon.source: "image://icons/icon-m-read?"+(root.transparent ? Theme.primaryColor : Theme.highlightDimmerColor)
                onClicked: {
                    show();
                    var remorse = pageStack.currentPage.remorse;
                    var name = pageStack.currentPage.objectName;
                    if (name == "tabs") {
                        remorse.execute(app.isNetvibes ? qsTr("Marking tabs as read") : qsTr("Marking folders as read"), function(){tabModel.setAllAsRead()});
                    } else if (name == "feeds") {
                        remorse.execute(qsTr("Marking feeds as read"), function(){feedModel.setAllAsRead()});
                    } else if (name == "entries") {
                        if (settings.viewMode==1 || settings.viewMode==5) {
                            remorse.execute(qsTr("Marking articles as read"), function(){entryModel.setAllAsRead()});
                            return;
                        }
                        if (settings.viewMode==3) {
                            remorse.execute(qsTr("Marking all articles as read"), function(){entryModel.setAllAsRead()});
                            return;
                        }
                        if (settings.viewMode==4) {
                            remorse.execute(
                                        settings.signinType<10 || settings.signinType>=20 ?
                                            qsTr("Marking all saved articles as read") :
                                            qsTr("Marking all starred articles as read")
                                        , function(){entryModel.setAllAsRead()});
                            return;
                        }
                        if (settings.viewMode==6) {
                            remorse.execute(qsTr("Marking all liked articles as read"), function(){entryModel.setAllAsRead()});
                            return;
                        }
                        if (settings.viewMode==7) {
                            remorse.execute(qsTr("Marking all shared articles as read"), function(){entryModel.setAllAsRead()});
                            return;
                        }

                        entryModel.setAllAsRead();
                    }
                }
            }
        }
    }

    MouseArea {
        enabled: !bar.visible && pageStack.currentPage.showBar
        anchors.bottom: parent.bottom; anchors.left: parent.left; anchors.right: parent.right
        height: root.height/2
        onClicked: root.show();
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
        target: flick

        onMovementStarted: {
            m.vector = 0;
            //m.lastContentY = 0.0;
            m.lastContentY=flick.contentY;
            m.initialContentY=flick.contentY;
        }

        onContentYChanged: {
            if (flick.moving) {
                var dInit = flick.contentY-m.initialContentY;
                var dLast = flick.contentY-m.lastContentY;
                var lastV = m.vector;
                if (dInit<-barShowMoveWidth)
                    root.show();
                if (dInit>barShowMoveWidthBack)
                    root.hide();
                if (dLast>barShowMoveWidthBack)
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
