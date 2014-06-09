/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Components project.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 1.1
import com.nokia.meego 1.0
import "constants.js" as UI
import "Theme.js" as Theme

Item {
    id: listItem

    signal clicked
    signal holded
    property alias pressed: mouseArea.pressed

    property int titleSize: UI.LIST_TILE_SIZE
    property int titleWeight: Font.Bold
    property string titleFont: (locale && locale.language == "fa") ? UI.FONT_FAMILY_FARSI : UI.FONT_FAMILY
    property color titleColor: theme.inverted ? UI.LIST_TITLE_COLOR_INVERTED : UI.LIST_TITLE_COLOR
    property color titleColorPressed: theme.inverted ? UI.LIST_TITLE_COLOR_PRESSED_INVERTED : UI.LIST_TITLE_COLOR_PRESSED

    property int subtitleSize: UI.LIST_SUBTILE_SIZE
    property int subtitleWeight: Font.Normal
    property string subtitleFont: (locale && locale.language == "fa") ? UI.FONT_FAMILY_LIGHT_FARSI : UI.FONT_FAMILY_LIGHT
    property color subtitleColor: theme.inverted ? UI.LIST_SUBTITLE_COLOR_INVERTED : UI.LIST_SUBTITLE_COLOR
    property color subtitleColorPressed: theme.inverted ? UI.LIST_SUBTITLE_COLOR_PRESSED_INVERTED : UI.LIST_SUBTITLE_COLOR_PRESSED

    property string iconSource: model.iconSource ? model.iconSource : ""
    property string titleText: model.title
    property string subtitleText: model.subtitle ? model.subtitle : ""

    property string iconId
    property bool iconVisible: false
    property int iconSize: UI.LIST_ICON_SIZE

    property bool showUnread: false
    property int unread

    height: UI.LIST_ITEM_HEIGHT
    width: parent.width

    BorderImage {
        id: background
        anchors.fill: parent
        // Fill page porders
        anchors.leftMargin: -UI.MARGIN_XLARGE
        anchors.rightMargin: -UI.MARGIN_XLARGE
        visible: mouseArea.pressed
        source: theme.inverted ? "image://theme/meegotouch-panel-inverted-background-pressed" : "image://theme/meegotouch-panel-background-pressed"
    }

    Item {
        anchors.fill: parent
        anchors.leftMargin: UI.LIST_ITEM_MARGIN
        //spacing: UI.LIST_ITEM_SPACING

        Image {
            id: image
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            visible: listItem.iconSource ? true : false
            width: visible ? iconSize : 0
            height: visible ? iconSize : 0
            source: listItem.iconSource ? listItem.iconSource : ""
        }

        Column {
            anchors.verticalCenter: parent.verticalCenter
            anchors { left: image.right; leftMargin: image.visible ? UI.LIST_ITEM_SPACING : 0;
                right: unreadbox.left; rightMargin: unreadbox.visible ? UI.LIST_ITEM_SPACING: 0 }


            Label {
                id: mainText
                anchors { left: parent.left; right: parent.right }
                text: listItem.titleText
                font.family: listItem.titleFont
                font.weight: listItem.titleWeight
                font.pixelSize: listItem.titleSize
                color: mouseArea.pressed ? listItem.titleColorPressed : listItem.titleColor
                maximumLineCount: 2
                elide: Text.ElideLeft
            }

            Label {
                id: subText
                anchors { left: parent.left; right: parent.right }
                text: listItem.subtitleText ? listItem.subtitleText : ""
                font.family: listItem.subtitleFont
                font.weight: listItem.subtitleWeight
                font.pixelSize: listItem.subtitleSize
                color: mouseArea.pressed ? listItem.subtitleColorPressed : listItem.subtitleColor
                maximumLineCount: 2
                elide: Text.ElideLeft

                visible: text != ""
            }
        }

        Rectangle {
            id: unreadbox

            anchors { right: parent.right; rightMargin: UI.LIST_ITEM_MARGIN }
            anchors.verticalCenter: parent.verticalCenter
            width: unreadlabel.width + 3 * Theme.paddingSmall
            height: unreadlabel.height + 2 * Theme.paddingSmall
            color: listItem.pressed ? Qt.rgba(0,0,0,0.1) : Qt.rgba(0,0,0,0.2)

            radius: 5
            visible: unread!==0 && showUnread

            Label {
                id: unreadlabel
                anchors.centerIn: parent
                text: unread
                color: listItem.pressed ? Theme.secondaryHighlightColor : Theme.highlightForegroundColor
            }
        }
    }
    MouseArea {
        id: mouseArea;
        anchors.fill: parent
        onClicked: listItem.clicked()
        onPressAndHold: listItem.holded()
    }

    Image {
        function handleIconId() {
            var prefix = "icon-m-"
            // check if id starts with prefix and use it as is
            // otherwise append prefix and use the inverted version if required
            if (iconId.indexOf(prefix) !== 0)
                iconId =  prefix.concat(iconId).concat(theme.inverted ?  "-inverse" : "");
            return "image://theme/" + iconId;
        }

        visible: iconVisible
        source: iconId ? handleIconId() : ""
        anchors {
            right: parent.right;
            verticalCenter: parent.verticalCenter;
        }
    }

}
