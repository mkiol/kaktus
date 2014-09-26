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
        onTriggered: {
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

    MouseArea {
        enabled: root.clickable
        anchors.fill: parent
        onClicked: {
            if (root.progress==0) {
                bar.open = true;
                selector.open = true;
            }
            if (root.progress==7) {
                bar.open = false;
                selector.open = false;
            }

            if (root.progress==8) {
                // Help finished
                settings.helpDone = true;
                root.open = false;
            } else {
                root.progress++;
            }
        }
    }

    Label {
        id: title
        anchors.left: parent.left; anchors.leftMargin: Theme.paddingLarge;
        anchors.top: parent.top; anchors.topMargin: Theme.paddingLarge
        font.pixelSize: Theme.fontSizeMedium
        color: Theme.highlightColor
        font.family: Theme.fontFamily
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignLeft
        text: qsTr("User Guide");
    }

    /*Label {
        anchors.right: parent.right; anchors.rightMargin: Theme.paddingLarge
        anchors.top: parent.top; anchors.topMargin: Theme.paddingLarge
        font.pixelSize: Theme.fontSizeMedium
        color: Theme.highlightColor
        font.family: Theme.fontFamily
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignLeft
        text: qsTr("Bottom Bar");
    }*/

    Rectangle {
        color: Theme.secondaryHighlightColor
        anchors.left: parent.left; anchors.right: parent.right
        anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
        anchors.top: title.bottom; anchors.topMargin: Theme.paddingMedium
        height: 1
    }

    /*IconButton {
        anchors.bottom: parent.bottom; anchors.bottomMargin: Theme.paddingMedium
        anchors.right: parent.right; anchors.rightMargin: Theme.paddingMedium
        icon.source: "image://theme/icon-m-close?"+Theme.highlightColor
        onClicked: {
                // Help finished
                // settings.helpDone = true;
                root.open = false;
        }
    }*/

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
            text: qsTr("This guide will explain you how to use Bottom Bar and View Modes.");
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

            text: qsTr( "Tap anywhere to continue.");
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
            //anchors.top: parent.top; anchors.topMargin: 2*Theme.paddingLarge
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: Theme.fontSizeLarge
            color: Theme.highlightColor
            font.family: Theme.fontFamily
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter

            text: qsTr("Bottom Bar lets you switch between 5 available View Modes.\n");
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
            //anchors.top: parent.top; anchors.topMargin: 2*Theme.paddingLarge
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: Theme.fontSizeLarge
            color: Theme.highlightColor
            font.family: Theme.fontFamily
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter

            text: qsTr("Mode #1 - Tabs & Feeds\n\nList all your tabs. "+
                       "Feeds are grouped by the tabs they belong to and articles are grouped in the feeds.");
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

            text: qsTr("Mode #2 - Only tabs\n\nList all your tabs. "+
                       "All articles are grouped only by the tabs they belong to.");
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
            //anchors.top: parent.top; anchors.topMargin: 2*Theme.paddingLarge
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: Theme.fontSizeLarge
            color: Theme.highlightColor
            font.family: Theme.fontFamily
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter

            text: qsTr("Mode #3 - All feeds\n\nList all articles from all your feeds. "+
                       "Items are ordered by publication date.");
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
            //anchors.top: parent.top; anchors.topMargin: 2*Theme.paddingLarge
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: Theme.fontSizeLarge
            color: Theme.highlightColor
            font.family: Theme.fontFamily
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter

            text: qsTr("Mode #4 - Saved\n\nList all the articles you have saved.");
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
            //anchors.top: parent.top; anchors.topMargin: 2*Theme.paddingLarge
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: Theme.fontSizeLarge
            color: Theme.highlightColor
            font.family: Theme.fontFamily
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter

            text: qsTr("Mode #5 - Slow\n\nList all articles from less frequently updated feeds. "+
                       "A feed is considered slow when it publishes less than 5 articles in a month.\n\n");
        }
    }

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
            //anchors.top: parent.top; anchors.topMargin: 2*Theme.paddingLarge
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: Theme.fontSizeLarge
            color: Theme.highlightColor
            font.family: Theme.fontFamily
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter

            text: qsTr("Bottom Bar also contains Network Indicator.\n\n"+
                       "This indicator enables you to switch between the Online and Offline mode. "+
                       "In the Offline mode, Kaktus will only use local cache to get web pages and images, so "+
                       "network connection won't be needed."
                       );
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
            //anchors.top: parent.top; anchors.topMargin: 2*Theme.paddingLarge
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: Theme.fontSizeLarge
            color: Theme.highlightColor
            font.family: Theme.fontFamily
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter

            text: qsTr("That's all!\n\n"+
                       "If you want to see this guide one more time, click on 'Show User Guide' in Settings page.");
        }
    }

    Item {
        id: bar

        property bool open: false
        anchors.bottom: parent.bottom
        anchors.left: parent.left; anchors.right: parent.right
        height: app.orientation==Orientation.Portrait ? app.panelHeightPortrait : app.panelHeightLandscape

        opacity: open ? 1.0 : 0.0
        visible: opacity > 0.0
        Behavior on opacity { FadeAnimation {duration: 300} }

        Image {
            property int off: 85
            property int size: 93
            source: "image://icons/bar?"+Theme.highlightBackgroundColor
            y: 0
            x: -off-4*size
            Behavior on x { NumberAnimation { duration: 200;easing.type: Easing.OutQuad } }
        }

        IconButton {
            id: vm0b
            x: 0*(width+Theme.paddingMedium)
            anchors.verticalCenter: parent.verticalCenter
            icon.source: "image://icons/vm0?"+Theme.highlightDimmerColor
            highlighted: true
        }

        IconButton {
            id: vm1b
            x: 1*(width+Theme.paddingMedium)
            anchors.verticalCenter: parent.verticalCenter
            icon.source: "image://icons/vm1?"+Theme.highlightDimmerColor
        }

        IconButton {
            id: vm3b
            x: 2*(width+Theme.paddingMedium)
            anchors.verticalCenter: parent.verticalCenter
            icon.source: "image://icons/vm3?"+Theme.highlightDimmerColor
        }

        IconButton {
            id: vm4b
            x: 3*(width+Theme.paddingMedium)
            anchors.verticalCenter: parent.verticalCenter
            icon.source: "image://icons/vm4?"+Theme.highlightDimmerColor
        }

        IconButton {
            id: vm5b
            x: 4*(width+Theme.paddingMedium)
            anchors.verticalCenter: parent.verticalCenter
            icon.source: "image://icons/vm5?"+(root.transparent ? Theme.primaryColor : Theme.highlightDimmerColor)
        }

        IconButton {
            id: offline
            anchors.right: parent.right; anchors.rightMargin: Theme.paddingSmall
            anchors.verticalCenter: parent.verticalCenter
            icon.source: settings.offlineMode ? "image://theme/icon-m-wlan-no-signal?"+(root.transparent ? Theme.primaryColor : Theme.highlightDimmerColor)
                                              : "image://theme/icon-m-wlan-4?"+(root.transparent ? Theme.primaryColor : Theme.highlightDimmerColor)
        }
    }

    Image {
        id: selector
        property bool open: false
        property int space: 92

        opacity: open ? 1.0 : 0.0
        visible: opacity > 0.0
        Behavior on opacity { FadeAnimation {duration: 300} }

        source: "image://icons/selector?"+Theme.primaryColor
        x: 8 + ((root.progress-2)*space)
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


}
