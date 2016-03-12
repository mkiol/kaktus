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

Dialog {
    id: root

    property bool showBar: false
    property int code

    canAccept: false

    allowedOrientations: {
        switch (settings.allowedOrientations) {
        case 1:
            return Orientation.Portrait;
        case 2:
            return Orientation.Landscape;
        }
        return Orientation.Landscape | Orientation.Portrait;
    }

    ActiveDetector {}

    SilicaFlickable {
        anchors {left: parent.left; right: parent.right }
        anchors {top: parent.top; bottom: parent.bottom }
        anchors.bottomMargin: {
            var size = 0;
            var d = app.orientation===Orientation.Portrait ? app.panelHeightPortrait : app.panelHeightLandscape;
            if (bar.open)
                size += d;
            if (progressPanel.open||progressPanelRemover.open||progressPanelDm.open)
                size += d;
            return size;
        }
        clip: true
        contentHeight: content.height

        Column {
            id: content
            anchors {
                left: parent.left
                right: parent.right
            }

            spacing: Theme.paddingSmall

            DialogHeader {
                acceptText : qsTr("Sign In")
            }

            Item {
                anchors { left: parent.left; right: parent.right}
                height: Math.max(icon.height, label.height)

                Image {
                    id: icon
                    anchors { right: label.left; rightMargin: Theme.paddingMedium }
                    source: "feedly.png"
                }

                Label {
                    id: label
                    anchors { right: parent.right; rightMargin: Theme.paddingLarge}
                    text: "Feedly"
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignRight
                    color: Theme.highlightColor
                    font.pixelSize: Theme.fontSizeSmall
                    y: Theme.paddingSmall/2
                }
            }

            Item {
                height: Theme.paddingMedium
                width: Theme.paddingMedium
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Sign in")
                onClicked: {
                    utils.resetQtWebKit();
                    fetcher.getConnectUrl(20);
                }
            }

            Item {
                height: Theme.itemSizeLarge
                width: Theme.itemSizeLarge
            }
        }
    }

}
