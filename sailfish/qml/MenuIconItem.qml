/****************************************************************************************
**
** Copyright (C) 2013 Jolla Ltd.
** Contact: Martin Jones <martin.jones@jollamobile.com>
** All rights reserved.
**
** This file is part of Sailfish Silica UI component package.
**
** You may use this file under the terms of BSD license as follows:
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of the Jolla Ltd nor the
**       names of its contributors may be used to endorse or promote products
**       derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
** ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
****************************************************************************************/

import QtQuick 2.0
import Sailfish.Silica 1.0

Item {
    id: menuItem
    property bool down
    property bool highlighted
    property alias text: label.text
    property alias icon: icon

    signal clicked

    property int __silica_menuitem
    property int _duration: 50
    property bool _invertColors
    on_InvertColorsChanged: _duration = 200

    x: Theme.horizontalPageMargin
    width: parent ? parent.width-2*Theme.horizontalPageMargin : Screen.width
    height: Theme.itemSizeSmall

    Label {
        id: label
        truncationMode: TruncationMode.Fade
        width: parent.width
        height: parent.height
        horizontalAlignment: implicitWidth > width && truncationMode != TruncationMode.None ? Text.AlignLeft : Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: parent.enabled ? ((parent.down || parent.highlighted) ^ parent._invertColors ? Theme.highlightColor : Theme.primaryColor)
                       : Theme.rgba(Theme.secondaryColor, 0.4)
        Behavior on color {
            SequentialAnimation {
                ColorAnimation { duration: menuItem._duration }
                ScriptAction { script: menuItem._duration = 50 }
            }
        }
    }

    Image {
        id: icon
        source: "image://icons/icon-m-item"
        height: Theme.iconSizeMedium
        width: Theme.iconSizeMedium
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
    }
}
