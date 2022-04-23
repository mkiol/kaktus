/*
  Copyright (C) 2017-2019 Michal Kosciesza <michal@mkiol.net>

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

ListItem {
    id: root

    property alias title: _title.text
    property alias icon: _icon.orgSource
    property bool showPlaceholder: false
    property int unreadCount: 0
    property bool small: false

    contentHeight: small ? Theme.itemSizeSmall : Theme.itemSizeMedium

    anchors {
        left: parent.left
        right: parent.right
    }

    IconPlaceholder {
        // placeholder
        id: placeholder
        anchors {
            left: parent.left
            leftMargin: Theme.horizontalPageMargin
            verticalCenter: parent.verticalCenter
        }
        visible: root.showPlaceholder &&
                 _icon.status !== Image.Ready &&
                 title.length > 0
        height: root.small ? Theme.iconSizeMedium * 0.8 : Theme.iconSizeMedium
        width: height
        text: title
    }

    CachedImage {
        id: _icon
        anchors {
            left: parent.left
            leftMargin: Theme.horizontalPageMargin
            verticalCenter: parent.verticalCenter
        }

        height: root.small ? Theme.iconSizeMedium * 0.8 : Theme.iconSizeMedium
        width: height
    }

    Label {
        id: _title

        truncationMode: TruncationMode.Fade

        anchors {
            left: !placeholder.visible && !_icon.visible ? parent.left : _icon.right
            right: unreadBox.visible ? unreadBox.left : parent.right
            leftMargin: Theme.horizontalPageMargin;
            rightMargin: Theme.horizontalPageMargin;
            verticalCenter: parent.verticalCenter
        }

        color: root.highlighted ? Theme.highlightColor : Theme.primaryColor
    }

    UnreadBox {
        id: unreadBox
        anchors {
            right: parent.right
            rightMargin: Theme.horizontalPageMargin
            verticalCenter: parent.verticalCenter
        }
        count: root.unreadCount
        visible: count > 0
    }
}
