/*
  Copyright (C) 2016 Michal Kosciesza <michal@mkiol.net>

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
    property int showTime: 5000

    // view modes
    property bool vmOpen: false

    // progress
    property bool busy: false
    property alias progressText: progressLabel.text
    property bool cancelable: true
    property real progress: 0.0
    signal cancelClicked

    // flick show/hide
    property real barShowMoveWidth: 20
    property real barShowMoveWidthBack: height
    property Flickable flick: null

    // other
    readonly property bool isPortrait: app.orientation == Orientation.Portrait || app.orientation == Orientation.PortraitInverted
    readonly property int stdHeight: isPortrait ? Theme.itemSizeMedium : 0.8 * Theme.itemSizeMedium

    height: root.stdHeight
    width: parent.width

    onOpenChanged: {
        if (!open)
            progress = 0.0
    }

    onBusyChanged: {
        open = busy
    }

    function show() {
        if (openable && pageStack.currentPage.showBar) {
            if (!open) {
                root.open = true
                root.vmOpen = false
            }
            timer.restart()
        }
    }

    function showAndEnable() {
        openable = true
        show()
    }

    function hide() {
        if (open && !busy) {
            root.open = false
            root.vmOpen = false
            timer.stop()
        }
    }

    function hideAndDisable() {
        hide()
        openable = false
    }

    Rectangle {
        id: bar

        width: parent.width
        height: parent.height
        //color: Theme.colorScheme ? "white" : "black"
        color: "transparent"

        clip: true

        Behavior on y {NumberAnimation { duration: 200; easing.type: Easing.OutQuad }}
        y: open ? 0 : height

        Rectangle {
            anchors.fill: parent
            opacity: 0.4
            color: Theme.colorScheme ? "white" : "black"
        }

        Image {
            anchors.fill: parent
            source: "image://theme/graphic-gradient-edge?" + Theme.primaryColor
        }

        MouseArea {
            enabled: root.open
            anchors.fill: parent
            onClicked: root.hide()
        }

        Item {
            anchors.fill: parent

            opacity: root.vmOpen && !root.busy ? 1.0 : 0.0
            visible: opacity > 0.0
            Behavior on opacity { FadeAnimation {} }

            IconButton {
                anchors.right: parent.right; anchors.rightMargin: Theme.paddingMedium
                anchors.verticalCenter: parent.verticalCenter
                icon.source: "image://theme/icon-m-dismiss?" + Theme.primaryColor
                onClicked: {
                    show()
                    root.vmOpen = !root.vmOpen
                }
            }

            Row {
                anchors.leftMargin: Theme.paddingMedium
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                spacing: Theme.paddingMedium * 0.8

                IconButton {
                    id: vm0b
                    anchors.verticalCenter: parent.verticalCenter
                    icon.source: "image://icons/icon-m-vm0?" + Theme.primaryColor
                    highlighted: settings.viewMode==0
                    onClicked: {
                        show()
                        root.vmOpen = false
                        if (!app.progress && settings.viewMode!=0) {
                            app.progress = true
                            settings.viewMode = 0
                        }
                    }
                }

                IconButton {
                    id: vm1b
                    anchors.verticalCenter: parent.verticalCenter
                    icon.source: "image://icons/icon-m-vm1?" + Theme.primaryColor
                    highlighted: settings.viewMode==1
                    onClicked: {
                        show()
                        root.vmOpen = false
                        if (!app.progress && settings.viewMode!=1) {
                            app.progress = true
                            settings.viewMode = 1
                        }
                    }
                }

                IconButton {
                    id: vm3b
                    anchors.verticalCenter: parent.verticalCenter
                    icon.source: "image://icons/icon-m-vm3?" + Theme.primaryColor
                    highlighted: settings.viewMode==3
                    onClicked: {
                        show()
                        root.vmOpen = false
                        if (!app.progress && settings.viewMode!=3) {
                            app.progress = true
                            settings.viewMode = 3
                        }
                    }
                }

                IconButton {
                    id: vm4b
                    anchors.verticalCenter: parent.verticalCenter
                    icon.source: "image://icons/icon-m-vm4?" + Theme.primaryColor
                    highlighted: settings.viewMode==4
                    onClicked: {
                        show()
                        root.vmOpen = false
                        if (!app.progress && settings.viewMode!=4) {
                            app.progress = true
                            settings.viewMode = 4
                        }
                    }
                }

                IconButton {
                    id: vm5b
                    visible: app.isNetvibes || (app.isOldReader && settings.showBroadcast)
                    anchors.verticalCenter: parent.verticalCenter
                    icon.source: app.isOldReader ? "image://icons/icon-m-vm6?" + Theme.primaryColor :
                                                   "image://icons/icon-m-vm5?" + Theme.primaryColor
                    highlighted: settings.viewMode==5 || settings.viewMode==6
                    onClicked: {
                        show()
                        root.vmOpen = false
                        if (!app.progress && (settings.viewMode!=5 || settings.viewMode!=6)) {
                            app.progress = true
                            if (settings.signinType >= 10)
                                settings.viewMode = 6
                            else
                                settings.viewMode = 5
                        }
                    }
                }
            }
        }

        Item {
            id: menu
            anchors.fill: parent

            opacity: !root.busy && !root.vmOpen && root.open ? 1.0 : 0.0
            visible: opacity > 0.0
            Behavior on opacity { FadeAnimation {} }

            IconButton {
                id: vmIcon
                anchors.right: parent.right; anchors.rightMargin: Theme.paddingMedium
                anchors.verticalCenter: parent.verticalCenter
                icon.source: "image://icons/icon-m-vm" + settings.viewMode + "?" + Theme.primaryColor
                highlighted: root.vmOpen
                onClicked: {
                    show()
                    root.vmOpen = !root.vmOpen
                }
            }

            IconButton {
                id: refreshIcon
                anchors.right: vmIcon.left; anchors.rightMargin: Theme.paddingMedium
                anchors.verticalCenter: parent.verticalCenter
                icon.source: "image://theme/icon-m-refresh?" + Theme.primaryColor
                enabled: !fetcher.busy && !dm.busy && !dm.removerBusy && dm.online
                onClicked: {
                    show()
                    fetcher.update()
                }

                Rectangle {
                    x: parent.height/8
                    y: parent.width/8
                    visible: !db.synced
                    width: Theme.paddingMedium
                    height: Theme.paddingMedium
                    radius: Theme.paddingMedium/2
                    color: Theme.errorColor
                }
            }

            IconButton {
                anchors.right: refreshIcon.left; anchors.rightMargin: Theme.paddingMedium
                anchors.verticalCenter: parent.verticalCenter

                visible: opacity > 0.0
                Behavior on opacity { FadeAnimation {} }
                opacity: (pageStack.currentPage.objectName === "entries" &&
                          settings.viewMode !==4 && settings.viewMode !==6 &&
                          settings.viewMode !==7) ? 1.0 : 0.0

                icon.source: "image://icons/icon-m-filter-" + settings.filter + "?" + Theme.primaryColor
                onClicked: {
                    show()
                    settings.filter = settings.filter === 2 ? 0 : settings.filter + 1
                }
            }

            /*IconButton {
                id: networkIcon
                anchors.left: markallIcon.right; anchors.leftMargin: Theme.paddingMedium
                //anchors.left: parent.left; anchors.leftMargin: Theme.paddingMedium
                anchors.verticalCenter: parent.verticalCenter
                enabled: !settings.offlineMode || (settings.offlineMode && dm.online)
                icon.source: "image://theme/icon-m-wlan" + (settings.offlineMode ? "-no-signal" : "") + "?" + root.iconColor
                onClicked: {
                    show()
                    if (settings.offlineMode) {
                        if (dm.online)
                            settings.offlineMode = false;
                        else
                            notification.show(qsTr("Cannot switch to online mode because network is disconnected."));
                    } else {
                        settings.offlineMode = true;
                    }
                }
            }*/

            IconButton {
                id: markallIcon
                visible: pageStack.currentPage.objectName != ""
                anchors.left: parent.left; anchors.leftMargin: Theme.paddingMedium
                //anchors.left: networkIcon.right; anchors.leftMargin: Theme.paddingMedium
                anchors.verticalCenter: parent.verticalCenter
                icon.source: "image://icons/icon-m-read?" + Theme.primaryColor
                onClicked: {
                    show()
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

        Item {
            id: progressPanel
            anchors.fill: parent

            opacity: root.busy && root.open ? 1.0 : 0.0
            visible: opacity > 0.0
            Behavior on opacity { FadeAnimation {} }

            Rectangle {
                anchors.bottom: parent.bottom; anchors.top: parent.top
                anchors.left: parent.left
                width: root.progress * parent.width
                color: Theme.highlightDimmerColor

                Behavior on width {
                    SmoothedAnimation {
                        velocity: 480; duration: 200
                    }
                }
            }

            Image {
                id: progressIcon
                height: 0.6*root.stdHeight; width: height
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: Theme.paddingMedium
                source: "image://theme/graphic-busyindicator-medium?" + Theme.primaryColor
                RotationAnimation on rotation {
                    loops: Animation.Infinite
                    from: 0
                    to: 360
                    duration: 1200
                    running: root.busy && Qt.application.active
                }
            }

            Label {
                id: progressLabel
                height: bar.height
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: progressIcon.right; anchors.right: root.cancelable ? closeButton.left : parent.right
                anchors.leftMargin: Theme.paddingMedium; anchors.rightMargin: Theme.paddingMedium
                font.pixelSize: Theme.fontSizeSmall
                font.family: Theme.fontFamily
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignLeft
            }

            IconButton {
                id: closeButton
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right; anchors.rightMargin: Theme.paddingMedium
                icon.source: "image://theme/icon-m-dismiss?" + Theme.primaryColor
                onClicked: root.cancelClicked()
                visible: root.cancelable
            }
        }
    }

    MouseArea {
        enabled: !root.open && pageStack.currentPage.showBar
        anchors.bottom: parent.bottom; anchors.left: parent.left; anchors.right: parent.right
        height: root.height/3
        onClicked: root.show()
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
