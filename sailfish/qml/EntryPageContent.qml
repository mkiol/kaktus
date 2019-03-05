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

SilicaFlickable {
    id: root

    property int index
    property string content
    property string image
    property bool busy: false
    property bool openable: false
    property alias textFormat: contentLabel.textFormat
    readonly property bool active: content != "" || image != ""

    signal clicked
    signal openClicked

    contentWidth: width
    contentHeight: column.height

    onIndexChanged: {
        scrollToTop();
    }

    Rectangle {
        anchors.fill: bgitem
        color: Theme.highlightDimmerColor
        visible: opacity > 0.0
        opacity: root.active ? 1.0 : 0.0
        Behavior on opacity { FadeAnimation {} }
    }

    BackgroundItem {
        id: bgitem
        width: column.width
        height: Math.max(column.height, root.height);
        onClicked: root.clicked();
        enabled: root.active

        Column {
            id: column
            spacing: Theme.paddingMedium
            width: root.width

            Image {
                id: entryImage
                anchors.left: parent.left;
                fillMode: Image.PreserveAspectFit
                width: sourceSize.width>=root.width ? root.width : sourceSize.width
                enabled: source!="" && status==Image.Ready &&
                         sourceSize.width > Theme.iconSizeMedium &&
                         sourceSize.height > Theme.iconSizeMedium
                visible: opacity>0
                opacity: enabled ? 1.0 : 0.0
                Behavior on opacity { FadeAnimation {} }
                source: {
                    if (root.image!="") {
                        return settings.offlineMode ? cserver.getUrlbyUrl(root.image) :
                               dm.online ? root.image : cserver.getUrlbyUrl(root.image);
                    } else {
                        return "";
                    }
                }
            }

            Item {
                height: 1
                width: parent.width
                visible: !entryImage.enabled
            }

            Label {
                id: contentLabel
                anchors.left: parent.left; anchors.right: parent.right;
                anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
                text: root.content
                wrapMode: Text.Wrap
                //textFormat: Text.PlainText
                //textFormat:  root.busy ? Text.PlainText : Text.StyledText
                linkColor: Theme.highlightColor
                font.pixelSize: Theme.fontSizeSmall
                visible: root.content!="" && !root.busy
                color: Theme.primaryColor
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                enabled: root.openable
                visible: enabled
                text: qsTr("Open")
                onClicked: {
                    if (root.openable)
                        root.openClicked();
                }
            }

            Item {
                height: 2 * Theme.paddingLarge
                width: parent.width
            }
        }
    }

    VerticalScrollDecorator {
        flickable: root
    }
}

