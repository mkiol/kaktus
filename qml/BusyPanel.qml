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



DockedPanel {
    id: root

    property alias text: label.text
    property bool cancelable: false
    property string icon: "icon-s-sync.png"
    property bool rotate: true

    signal cancel()

    onOpenChanged: {
        console.log("open="+open);
        if (!open) {
            icon = "icon-s-sync.png";
            rotate = true;
            cancelable = false;
            icon.rotation = 0;
        }
    }

    width:  parent.width
    height: 120

    dock: Dock.Bottom

    /*Rectangle {
        id: shadow
        anchors.fill: parent
        color: Theme.rgba(Theme.highlightBackgroundColor, 0.4)
    }

    OpacityRampEffect {
        id: effect
        slope: 2
        offset: 0.5
        direction: OpacityRamp.BottomToTop
        sourceItem: shadow
    }*/

    Rectangle {
        id: background
        anchors.left: parent.left; anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 50

        color: Theme.highlightBackgroundColor

        BackgroundItem {
            anchors.fill: background
            onClicked: {
                if (cancelable)
                    cancel();
            }
        }
    }

    Image {
        id: icon
        anchors.left: parent.left; anchors.leftMargin: Theme.paddingMedium;
        anchors.verticalCenter: background.verticalCenter
        width: Theme.iconSizeSmall
        height: Theme.iconSizeSmall
        source: root.icon

        RotationAnimation on rotation {
            loops: Animation.Infinite
            from: 0
            to: 360
            duration: 1000
            running: root.open && root.rotate
        }
    }

    Label {
        id: label
        font.pixelSize: Theme.fontSizeSmall
        font.family: Theme.fontFamily
        color: "black"
        anchors.verticalCenter: background.verticalCenter
        anchors.left: icon.right; anchors.leftMargin: Theme.paddingMedium
        opacity: 0.8

    }

    Label {
        visible: root.cancelable
        text: "Tap to cancel"
        font.pixelSize: Theme.fontSizeSmall
        font.family: Theme.fontFamily
        color: "black"
        anchors.verticalCenter: background.verticalCenter
        anchors.right: parent.right; anchors.rightMargin: Theme.paddingMedium
        opacity: 0.8
    }

}
