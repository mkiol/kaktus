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

Rectangle {
    id: root
    property bool open: false
    property bool clickable: false
    property int progress: 0
    visible: opacity > 0.0
    opacity: root.open ? 1.0 : 0.0
    color: "black"
    Behavior on opacity { FadeAnimation {duration: 500} }

    function show() {
        root.progress = 0;
        selector.open = false;
        bar.open = false;
        clickable = false;
        root.open = true;
        timer2.start();
    }

    function showDelayed() {
        timer1.start();
    }

    Timer {
        id: timer1
        interval: 3000
        onTriggered: {s
            root.show();
        }
    }

    Timer {
        id: timer2
        interval: 2000
        onTriggered: {
            root.clickable = true;
        }
    }

    Row {
        anchors.top: parent.top; anchors.topMargin: 2*Theme.paddingLarge
        spacing: (3*parent.width/4 - 9*dot.width)/8
        x: parent.width/8

        Dot {
            id: dot
            active: root.progress>=0
        }

        Dot {
            active: root.progress>=1
        }

        Dot {
            active: root.progress>=2
        }

        Dot {
            active: root.progress>=3
        }

        Dot {
            active: root.progress>=4
        }

        Dot {
            active: root.progress>=5
        }

        Dot {
            active: root.progress>=6
        }

        Dot {
            active: root.progress>=7
        }

        Dot {
            active: root.progress>=8
        }
    }

    Item {
        id: content0
        property bool open: root.progress==0
        anchors.fill: parent
        opacity: open ? 1.0 : 0.0
        visible: opacity > 0.0
        Behavior on opacity { FadeAnimation {duration: 400} }

        Label {
            anchors.left: parent.left; anchors.right: parent.right
            anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: Theme.fontSizeLarge
            color: Theme.highlightColor
            font.family: Theme.fontFamily
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            //textFormat: Text.StyledText

            text: qsTr("This guide will explain you how to use bottom bar and view modes.");
        }

        Label {
            y: parent.height*(3/4);
            opacity: root.clickable ? 1.0 : 0.0
            visible: opacity > 0.0
            Behavior on opacity { FadeAnimation {duration: 400} }
            anchors.left: parent.left; anchors.right: parent.right
            font.pixelSize: Theme.fontSizeLarge
            color: Theme.secondaryHighlightColor
            font.family: Theme.fontFamily
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter

            text: qsTr("Tap anywhere to continue.");
        }
    }

    Item {
        id: content1
        property bool open: root.progress==1
        anchors.fill: parent
        opacity: open ? 1.0 : 0.0
        visible: opacity > 0.0
        Behavior on opacity { FadeAnimation {duration: 400} }

        Label {
            anchors.left: parent.left; anchors.right: parent.right
            anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: Theme.fontSizeLarge
            color: Theme.highlightColor
            font.family: Theme.fontFamily
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            //textFormat: Text.StyledText

            text:  qsTr("Bottom bar lets you switch between 5 view modes.\n");
        }
    }

    Item {
        id: content2
        property bool open: root.progress==2
        anchors.fill: parent
        opacity: open ? 1.0 : 0.0
        visible: opacity > 0.0
        Behavior on opacity { FadeAnimation {duration: 400} }

        Label {
            anchors.left: parent.left; anchors.right: parent.right
            anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: Theme.fontSizeLarge
            color: Theme.highlightColor
            font.family: Theme.fontFamily
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            //textFormat: Text.StyledText

            text: app.isNetvibes ? qsTr("Mode #1\n\nLists all your tabs. Feeds are grouped by the tabs they belong to and articles are grouped in the feeds.") :
                                   qsTr("Mode #1\n\nLists all your folders. Feeds are grouped by the folders they belong to and articles are grouped in the feeds.")
        }
    }

    Item {
        id: content3
        property bool open: root.progress==3
        anchors.fill: parent
        opacity: open ? 1.0 : 0.0
        visible: opacity > 0.0
        Behavior on opacity { FadeAnimation {duration: 400} }

        Label {
            anchors.left: parent.left; anchors.right: parent.right
            anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: Theme.fontSizeLarge
            color: Theme.highlightColor
            font.family: Theme.fontFamily
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            text: app.isNetvibes ? qsTr("Mode #2\n\nLists all your tabs. Articles are grouped by the tabs they belong to.") :
                                   qsTr("Mode #2\n\nLists all your folders. Articles are grouped by the folders they belong to.");
        }
    }

    Item {
        id: content4
        property bool open: root.progress==4
        anchors.fill: parent
        opacity: open ? 1.0 : 0.0
        visible: opacity > 0.0
        Behavior on opacity { FadeAnimation {duration: 400} }

        Label {
            anchors.left: parent.left; anchors.right: parent.right
            anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: Theme.fontSizeLarge
            color: Theme.highlightColor
            font.family: Theme.fontFamily
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            text: qsTr("Mode #3\n\nLists all articles from all your feeds in one list. Items are ordered by publication date.");
        }
    }

    Item {
        id: content5
        property bool open: root.progress==5
        anchors.fill: parent
        opacity: open ? 1.0 : 0.0
        visible: opacity > 0.0
        Behavior on opacity { FadeAnimation {duration: 400} }

        Label {
            anchors.left: parent.left; anchors.right: parent.right
            anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: Theme.fontSizeLarge
            color: Theme.highlightColor
            font.family: Theme.fontFamily
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            text: app.isNetvibes ? qsTr("Mode #4\n\nLists all articles you have saved.") :
                                   qsTr("Mode #4\n\nLists all articles you have starred.")
        }
    }

    Item {
        id: content6
        property bool open: root.progress==6
        anchors.fill: parent
        opacity: open ? 1.0 : 0.0
        visible: opacity > 0.0
        Behavior on opacity { FadeAnimation {duration: 400} }

        Label {
            anchors.left: parent.left; anchors.right: parent.right
            anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: Theme.fontSizeLarge
            color: Theme.highlightColor
            font.family: Theme.fontFamily
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            text: app.isNetvibes ? qsTr("Mode #5 \"Slow\"\n\nList articles from less frequently updated feeds. "+
                                        "A feed is considered \"slow\" when it publishes less than 5 articles in a month.") :
                                   qsTr("Mode #5\n\nLists all articles you have liked.");
        }
    }

    /*Item {
        id: content7
        property bool open: root.progress==7
        anchors.fill: parent
        opacity: open ? 1.0 : 0.0
        visible: opacity > 0.0
        Behavior on opacity { FadeAnimation {duration: 400} }

        Label {
            anchors.left: parent.left; anchors.right: parent.right
            anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: Theme.fontSizeLarge
            color: Theme.highlightColor
            font.family: Theme.fontFamily
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            text: qsTr("Bottom bar also contains network indicator.\n\n"+
                       "This indicator enables you to switch between the online and offline mode. "+
                       "In the offline mode, Kaktus will only use local cache to get web pages and images, so "+
                       "network connection won't be needed."
                       );
        }
    }*/

    Item {
        id: content7
        property bool open: root.progress==7
        anchors.fill: parent
        opacity: open ? 1.0 : 0.0
        visible: opacity > 0.0
        Behavior on opacity { FadeAnimation {duration: 400} }

        Label {
            anchors.left: parent.left; anchors.right: parent.right
            anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: Theme.fontSizeLarge
            color: Theme.highlightColor
            font.family: Theme.fontFamily
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            text: app.isNetvibes ? qsTr("Bottom bar also contains \"Mark as read\" button.\n\nWith this button you can quickly mark all your tabs, feeds or articles as read.") :
                                   qsTr("Bottom bar also contains \"Mark as read\" button.\n\nWith this button you can quickly mark all your folders, feeds or articles as read.");
        }
    }

    Item {
        id: content8
        property bool open: root.progress==8
        anchors.fill: parent
        opacity: open ? 1.0 : 0.0
        visible: opacity > 0.0
        Behavior on opacity { FadeAnimation {duration: 400} }

        Label {
            anchors.left: parent.left; anchors.right: parent.right
            anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: Theme.fontSizeLarge
            color: Theme.highlightColor
            font.family: Theme.fontFamily
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            text: qsTr("Pull-down menu contains network mode switcher.\n\n"+
                       "This switcher enables you to change between the online and offline mode. "+
                       "In the offline mode, Kaktus will only use local cache to get web pages and images, so "+
                       "network connection won't be needed."
                       );
        }
    }

    Item {
        id: content9
        property bool open: root.progress==9
        anchors.fill: parent
        opacity: open ? 1.0 : 0.0
        visible: opacity > 0.0
        Behavior on opacity { FadeAnimation {duration: 400} }

        Label {
            anchors.left: parent.left; anchors.right: parent.right
            anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: Theme.fontSizeLarge
            color: Theme.highlightColor
            font.family: Theme.fontFamily
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            textFormat: Text.StyledText
            text: qsTr("That's all!<br/><br/>"+
                       "If you want to see this guide one more time, click on\n"+
                       "<i>Show User Guide</i>\non the settings page.");
        }
    }

    Item {
        id: bar

        property bool open: false
        anchors.bottom: parent.bottom
        anchors.left: parent.left; anchors.right: parent.right
        height: app.barHeight

        opacity: bar.open ? 1.0 : 0.0
        visible: opacity > 0.0
        Behavior on opacity { FadeAnimation {duration: 300} }

        property int off: 5
        property int size: vm0b.width + Theme.paddingMedium

        Rectangle {
            id: leftRect
            anchors.left: parent.left
            anchors.top: parent.top; anchors.bottom: parent.bottom
            color: Theme.highlightBackgroundColor
            width: parent.width
        }

        Item {
            property int off: -bar.height/10
            anchors.left: parent.left; anchors.right: parent.right
            anchors.bottom: parent.bottom; height: parent.height

            IconButton {
                id: vm0b
                x: 0*(width+Theme.paddingMedium)
                y: ((parent.height-height)/2) + (highlighted ? parent.off : 0)
                icon.source: "image://icons/icon-m-vm0?"+Theme.highlightDimmerColor
                //highlighted: true
            }

            IconButton {
                id: vm1b
                x: 1*(width+Theme.paddingMedium)
                y: ((parent.height-height)/2) + (highlighted ? parent.off : 0)
                icon.source: "image://icons/icon-m-vm1?"+Theme.highlightDimmerColor
            }

            IconButton {
                id: vm3b
                x: 2*(width+Theme.paddingMedium)
                y: ((parent.height-height)/2) + (highlighted ? parent.off : 0)
                icon.source: "image://icons/icon-m-vm3?"+Theme.highlightDimmerColor
            }

            IconButton {
                id: vm4b
                x: 3*(width+Theme.paddingMedium)
                y: ((parent.height-height)/2) + (highlighted ? parent.off : 0)
                icon.source: "image://icons/icon-m-vm4?"+Theme.highlightDimmerColor
            }

            IconButton {
                id: vm5b
                x: 4*(width+Theme.paddingMedium)
                visible: app.isNetvibes || (app.isOldReader && settings.showBroadcast)
                y: ((parent.height-height)/2) + (highlighted ? parent.off : 0)
                icon.source: app.isOldReader ? "image://icons/icon-m-vm6?"+(root.transparent ? Theme.primaryColor : Theme.highlightDimmerColor) :
                                               "image://icons/icon-m-vm5?"+(root.transparent ? Theme.primaryColor : Theme.highlightDimmerColor)
            }

            IconButton {
                id: readb
                anchors.right: parent.right; anchors.rightMargin: Theme.paddingSmall
                anchors.verticalCenter: parent.verticalCenter
                icon.source: "image://icons/icon-m-read?"+(root.transparent ? Theme.primaryColor : Theme.highlightDimmerColor)
            }
        }
    }

    Image {
        id: selector
        property bool open: false
        property int space: 92
        visible: open
        source: "image://icons/icon-m-selector?"+Theme.primaryColor
        //x: root.progress!=7 ? 8 + ((root.progress-2)*space) : parent.width-space+Theme.paddingMedium
        x: root.progress==2 ? vm0b.x :
           root.progress==3 ? vm1b.x :
           root.progress==4 ? vm3b.x :
           root.progress==5 ? vm4b.x :
           root.progress==6 ? vm5b.x : readb.x

        y: app.orientation==Orientation.Portrait ? parent.height-(app.panelHeightPortrait+height)/2 :
                                                   parent.height-(app.panelHeightLandscape+height)/2
        Behavior on x { NumberAnimation { duration: 200;easing.type: Easing.OutQuad } }

        PropertyAnimation on opacity {
            loops: Animation.Infinite
            from: 0.0
            to: 1.0
            duration: 1200
            running: root.open && Qt.application.active
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (root.clickable) {
                if (root.progress==0) {
                    bar.open = true;
                }

                if (root.progress==1) {
                    selector.open = true;
                }

                /*if (root.progress==5 && app.isFeedly) {
                    root.progress++;
                }*/

                if (root.progress==7) {
                    bar.open = false;
                    selector.open = false;
                }

                if (root.progress==9) {
                    // Help finished
                    settings.helpDone = true;
                    root.open = false;
                } else {
                    root.progress++;
                }
            }
        }
    }

}
