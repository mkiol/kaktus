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


Page {
    id: root

    property bool showBar: true

    allowedOrientations: {
        switch (settings.allowedOrientations) {
        case 1:
            return Orientation.Portrait;
        case 2:
            return Orientation.Landscape;
        }
        return Orientation.Landscape | Orientation.Portrait;
    }

    property string title
    property int index

    ActiveDetector {
        onActivated: { feedModel.updateFlags();}
        onInit: { bar.flick = listView; }
    }

    RemorsePopup {id: remorse}

    SilicaListView {
        id: listView
        model: feedModel

        anchors { top: parent.top; left: parent.left; right: parent.right }
        clip:true

        height: app.flickHeight

        PageMenu {
            id: menu
            showAbout: settings.viewMode==2 ? true : false
            showMarkAsRead: false
            showMarkAsUnread: false

            onMarkedAsRead: {
                //pageStack.push(Qt.resolvedUrl("ReadAllDialog.qml"),{"type": 1});
                remorse.execute(qsTr("Marking feeds as read"), function(){feedModel.setAllAsRead()});
            }
            /*onMarkedAsUnread: {
                //pageStack.push(Qt.resolvedUrl("UnreadAllDialog.qml"),{"type": 1});
                remorse.execute(qsTr("Marking feeds as unread"), function(){feedModel.setAllAsUnread()});
            }*/

            onActiveChanged: {
                if (active && settings.viewMode!=2) {
                    showMarkAsRead = feedModel.countUnread()!=0;
                    //showMarkAsUnread = feedModel.countRead()!=0;
                }
            }
        }

        header: PageHeader {
            title: settings.viewMode==2 ? qsTr("Feeds") : root.title
        }

        delegate: ListItem {
            id: listItem

            property bool last: model.uid=="last"
            enabled: !last

            contentHeight: last ?
                             app.orientation==Orientation.Portrait ? app.panelHeightPortrait : app.panelHeightLandscape :
                             Math.max(item.height, image.height) + 2 * Theme.paddingMedium

            Rectangle {
                anchors.top: parent.top; anchors.right: parent.right
                width: Theme.paddingSmall; height: item.height
                visible: model.fresh && !listItem.last
                radius: 10

                gradient: Gradient {
                    GradientStop { position: 0.0; color: Theme.rgba(Theme.highlightColor, 0.4) }
                    GradientStop { position: 1.0; color: Theme.rgba(Theme.highlightColor, 0.0) }
                }
            }

            Column {
                id: item
                spacing: 0.5*Theme.paddingSmall
                anchors.verticalCenter: parent.verticalCenter
                //anchors.left: image.visible ? image.right : parent.left
                anchors.left: image.visible ? image.right : imagePlaceholder.right
                anchors.right: unreadbox.visible ? unreadbox.left : parent.right
                visible: !listItem.last

                Label {
                    id: itemLabel
                    wrapMode: Text.AlignLeft
                    anchors.left: parent.left; anchors.right: parent.right;
                    anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
                    font.pixelSize: Theme.fontSizeMedium
                    text: title
                    color: listItem.down ?
                               (model.unread ? Theme.highlightColor : Theme.secondaryHighlightColor) :
                               (model.unread ? Theme.primaryColor : Theme.secondaryColor)
                }
            }

            Rectangle {
                id: unreadbox
                anchors.right: parent.right; anchors.rightMargin: Theme.paddingLarge
                y: Theme.paddingSmall
                width: unreadlabel.width + 2 * Theme.paddingSmall
                height: unreadlabel.height + 2 * Theme.paddingSmall
                color: Theme.rgba(Theme.highlightBackgroundColor, 0.2)
                radius: 5
                visible: model.unread!=0 && !listItem.last

                Label {
                    id: unreadlabel
                    anchors.centerIn: parent
                    text: model.unread
                    color: Theme.highlightColor
                }

            }

            Rectangle {
                id: imagePlaceholder
                width: visible ? 1.2*Theme.iconSizeSmall : 0
                height: width
                anchors.left: parent.left
                y: Theme.paddingMedium
                visible: !image.visible && !listItem.last

                Label {
                    id: imagePlaceholderLabel
                    anchors.centerIn: parent
                    text: title.substring(0,1).toUpperCase()
                }

                Component.onCompleted: {
                    var r = title.length>0 ? (Math.abs(title.charCodeAt(0)-65)/57)%1 : 1;
                    var g = title.length>1 ? (Math.abs(title.charCodeAt(1)-65)/57)%1 : 1;
                    var b = title.length>2 ? (Math.abs(title.charCodeAt(2)-65)/57)%1 : 1;
                    imagePlaceholder.color = Qt.rgba(r,g,b,0.9);
                    imagePlaceholderLabel.color = (r+g+b)>1.5 ? Theme.highlightDimmerColor : Theme.primaryColor;
                }
            }

            Rectangle {
                anchors.fill: image
                //color: Theme.secondaryColor
                visible: image.visible
                color: "white"
                //opacity: 0.1
            }

            Image {
                id: image
                width: visible ? 1.2*Theme.iconSizeSmall : 0
                height: width
                anchors.left: parent.left; //anchors.leftMargin: Theme.paddingLarge
                visible: status!=Image.Error && status!=Image.Null && !listItem.last
                y: Theme.paddingMedium
            }

            Connections {
                target: settings
                onShowTabIconsChanged: {
                    if (model.icon!="")
                        image.source = cache.getUrlbyUrl(model.icon);
                    else
                        image.source = "";
                }
            }

            Component.onCompleted: {
                if (model.icon!="")
                    image.source = cache.getUrlbyUrl(model.icon);
                else
                    image.source = "";
            }

            onClicked: {
                if (!listItem.last) {
                    utils.setEntryModel(uid);
                    pageStack.push(Qt.resolvedUrl("EntryPage.qml"),{"title": title, "index": model.index, "readlater": false});
                }
            }

            showMenuOnPressAndHold: !listItem.last && model.unread+model.read>0

            menu: ContextMenu {
                id: contextMenu
                MenuItem {
                    text: qsTr("Mark all as read")
                    enabled: model.unread!=0
                    visible: enabled
                    onClicked: {
                        feedModel.markAsRead(model.index);
                    }
                }
                MenuItem {
                    text: qsTr("Mark all as unread")
                    enabled: model.read!=0 && settings.signinType<10
                    visible: enabled
                    onClicked: {
                        feedModel.markAsUnread(model.index);
                    }
                }
            }
        }

        ViewPlaceholder {
            id: placeholder
            enabled: listView.count == 0
            text: fetcher.busy ? qsTr("Wait until Sync finish.") : qsTr("No feeds")
        }

        VerticalScrollDecorator {
            flickable: listView
        }
    }
}
