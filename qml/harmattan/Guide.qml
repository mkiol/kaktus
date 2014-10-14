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

import QtQuick 1.1
import com.nokia.meego 1.0

import "Theme.js" as Theme

Rectangle {
    id: root
    property bool open: false
    property bool clickable: false
    property int progress: 0

    x: 0; y: 0; width: parent.width; height: parent.height
    visible: opacity > 0.0
    opacity: root.open ? 1.0 : 0.0
    color: "black"
    Behavior on opacity { NumberAnimation {duration: 500} }

    function show() {
        root.progress = 0;
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
            if (root.progress==8) {
                // Help finished
                settings.helpDone = true;
                root.open=false;
            } else {
                root.progress++;
            }
        }
    }

    Label {
        id: title
        anchors.left: parent.left; anchors.leftMargin: Theme.paddingLarge;
        anchors.top: parent.top
        anchors.topMargin: isPortrait ? UiConstants.HeaderDefaultHeightPortrait:
                                        UiConstants.HeaderDefaultHeightLandscape
        font.pixelSize: Theme.fontSizeSmall
        color: Theme.highlightColor
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignLeft
        text: qsTr("User Guide");
    }

    Rectangle {
        color: Theme.highlightColor
        anchors.left: parent.left; anchors.right: parent.right
        anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
        anchors.top: title.bottom; anchors.topMargin: Theme.paddingMedium
        opacity: 0.5
        height: 1
    }

    Item {
        id: content0
        property bool open: root.progress==0
        anchors.fill: parent
        opacity: open ? 1.0 : 0.0
        visible: opacity > 0.0
        Behavior on opacity { NumberAnimation {duration: 400} }

        Label {
            anchors.left: parent.left; anchors.right: parent.right
            anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: Theme.fontSizeLarge
            color: Theme.highlightColor
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            //textFormat: Text.StyledText

            text: qsTr("This guide will explain you how to use bottom bar and view modes.");
        }

        Label {
            y: parent.height*(3/4);
            opacity: root.clickable ? 1.0 : 0.0
            visible: opacity > 0.0
            Behavior on opacity { NumberAnimation {duration: 400} }
            anchors.left: parent.left; anchors.right: parent.right
            font.pixelSize: Theme.fontSizeMedium
            color: Theme.secondaryHighlightColor
            //font: UiConstants.HeaderFont
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
        Behavior on opacity { NumberAnimation {duration: 400} }

        Label {
            anchors.left: parent.left; anchors.right: parent.right
            anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: Theme.fontSizeLarge
            color: Theme.highlightColor
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            //textFormat: Text.StyledText

            text: qsTr("Bottom bar lets you switch between 5 available view modes.\n");
        }
    }

    Item {
        id: content2
        property bool open: root.progress==2
        anchors.fill: parent
        opacity: open ? 1.0 : 0.0
        visible: opacity > 0.0
        Behavior on opacity { NumberAnimation {duration: 400} }

        Label {
            anchors.left: parent.left; anchors.right: parent.right
            anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: Theme.fontSizeLarge
            color: Theme.highlightColor
            //font: UiConstants.HeaderFont
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            //textFormat: Text.StyledText

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
        Behavior on opacity { NumberAnimation {duration: 400} }

        Label {
            anchors.left: parent.left; anchors.right: parent.right
            anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: Theme.fontSizeLarge
            color: Theme.highlightColor
            //font: UiConstants.HeaderFont
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            //textFormat: Text.StyledText

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
        Behavior on opacity { NumberAnimation {duration: 400} }

        Label {
            anchors.left: parent.left; anchors.right: parent.right
            anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: Theme.fontSizeLarge
            color: Theme.highlightColor
            //font: UiConstants.HeaderFont
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            //textFormat: Text.StyledText

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
        Behavior on opacity { NumberAnimation {duration: 400} }

        Label {
            anchors.left: parent.left; anchors.right: parent.right
            anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: Theme.fontSizeLarge
            color: Theme.highlightColor
            //font: UiConstants.HeaderFont
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
        Behavior on opacity { NumberAnimation {duration: 400} }

        Label {
            anchors.left: parent.left; anchors.right: parent.right
            anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: Theme.fontSizeLarge
            color: Theme.highlightColor
            //font: UiConstants.HeaderFont
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            //textFormat: Text.StyledText

            text: qsTr("Mode #5 - Slow\n\nList all articles from less frequently updated feeds. "+
                       "A feed is considered slow when it publishes less than 5 articles in a month.");
        }
    }

    Item {
        id: content7
        property bool open: root.progress==7
        anchors.fill: parent
        opacity: open ? 1.0 : 0.0
        visible: opacity > 0.0
        Behavior on opacity { NumberAnimation {duration: 400} }

        Label {
            anchors.left: parent.left; anchors.right: parent.right
            anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: Theme.fontSizeLarge
            color: Theme.highlightColor
            //font: UiConstants.HeaderFont
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            //textFormat: Text.StyledText

            text: qsTr("Bottom bar also contains network indicator.\n\n"+
                       "This indicator enables you to switch between the online and offline mode. "+
                       "In the offline mode, Kaktus will only use local cache to get web pages and images, so "+
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
        Behavior on opacity { NumberAnimation {duration: 400} }

        Label {
            anchors.left: parent.left; anchors.right: parent.right
            anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: Theme.fontSizeLarge
            color: Theme.highlightColor
            //font: UiConstants.HeaderFont
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            textFormat: Text.StyledText

            text: qsTr("That's all!<br/><br/>"+
                       "If you want to see this guide one more time, click on "+
                       "<i>Show User Guide</i> on the settings page.");
        }
    }

    ToolBar {
        id: toolbar

        property bool open: root.progress>0 && root.progress<8
        enabled: open
        opacity: open ? 1.0 : 0.0
        visible: opacity > 0.0
        Behavior on opacity { NumberAnimation { duration: 300 } }

        anchors.bottom: parent.bottom
        tools: ToolBarLayout {

            ToolIcon {
                iconId: pageStack.depth > 1 ? "toolbar-back" : "toolbar-close"
            }

            ToolIcon {
                iconId: "toolbar-refresh"
            }

            ToolIcon {
                iconSource: "vm0.png"
            }

            ToolIcon {
                iconSource: settings.offlineMode ?
                                (theme.inverted ? "offline-inverted.png" : "offline.png") :
                                (theme.inverted ? "online-inverted.png" : "online.png")
            }

            ToolIcon {
                iconId: "toolbar-view-menu"
            }
        }
    }

    Item {
        id: bar

        property bool open: root.progress>0 && root.progress<7

        height: app.isPortrait ? Theme.navigationBarPortrait : Theme.navigationBarLanscape

        anchors.bottom: parent.bottom
        anchors.bottomMargin: app.isPortrait ? Theme.navigationBarPortrait : Theme.navigationBarLanscape
        anchors.right: parent.right
        anchors.left: parent.left

        enabled: open
        opacity: open ? 1.0 : 0.0
        visible: opacity > 0.0
        Behavior on opacity { NumberAnimation { duration: 300 } }

        Rectangle {
            id: background
            anchors.fill: parent
            color: Qt.rgba(0.1,0.1,0.1, 0.9)
        }

        Row {
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: spacing
            spacing: (parent.width-5*icon.width)/6

            ToolIcon {
                id: icon
                iconSource: "vm0.png"
            }

            ToolIcon {
                iconSource: "vm1.png"
            }

            ToolIcon {
                iconSource: "vm3.png"
            }

            ToolIcon {
                iconSource: "vm4.png"
            }

            ToolIcon {
                iconSource: "vm5.png"
            }
        }
    }

    Selector {
        id: selector
        open: root.progress>0 && root.progress<8

        x: {
            switch (root.progress) {
            case 0:
            case 1:
                return app.isPortrait ? 207 : 392;
            case 2:
                return app.isPortrait ? 18 : 84;
            case 3:
                return app.isPortrait ? 112 : 242;
            case 4:
                return app.isPortrait ? 207 : 397;
            case 5:
                return app.isPortrait ? 299 : 553;
            case 6:
                return app.isPortrait ? 393 : 708;
            case 7:
            default:
                return app.isPortrait ? 309 : 589;
            }
        }

        y: {
            switch (root.progress) {
            case 0:
            case 1:
                return app.isPortrait ? 788 : 423;
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
                return app.isPortrait ? 715 : 365;
            case 7:
            default:
                return app.isPortrait ? 786 : 420;
            }
        }
    }

}
