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

    ActiveDetector {
        onActivated: {
            tabModel.updateFlags();
        }
        onInit: {
            bar.flick = listView;
        }
    }

    RemorsePopup {id: remorse}

    SilicaListView {
        id: listView
        model: tabModel

        anchors { top: parent.top; left: parent.left; right: parent.right }

        height: app.flickHeight

        clip:true

        PageMenu {
            id: menu
            showAbout: true
            showMarkAsRead: false
            showMarkAsUnread: false

            /*onMarkedAsRead: {
                remorse.execute(qsTr("Marking all tabs as read"), function(){tabModel.setAllAsRead()});
            }
            onMarkedAsUnread: {
                remorse.execute(qsTr("Marking all tabs as unread"), function(){tabModel.setAllAsUnread()});
            }

            onActiveChanged: {
                if (active) {
                    showMarkAsRead = tabModel.countUnread()!=0;
                    //showMarkAsUnread = !showMarkAsRead
                }
            }*/
        }

        header: PageHeader {
            title: qsTr("Tabs")
        }

        delegate: Item {

            anchors.left: parent.left; anchors.right: parent.right
            height: listItem.height

            ListItem {
                id: listItem

                property bool last: model.uid=="last"
                enabled: !last

                anchors.top: parent.top
                contentHeight: last ?
                                 app.orientation==Orientation.Portrait ? app.panelHeightPortrait : app.panelHeightLandscape :
                                 Math.max(item.height, image.height) + 2 * Theme.paddingMedium;

                Rectangle {
                    //anchors.top: parent.top; anchors.left: parent.left
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
                    anchors.left: image.visible ? image.right : parent.left
                    anchors.right: unreadbox.visible ? unreadbox.left : parent.right
                    visible: !listItem.last

                    Label {
                        wrapMode: Text.AlignLeft
                        anchors.left: parent.left; anchors.right: parent.right;
                        anchors.leftMargin: Theme.paddingLarge; anchors.rightMargin: Theme.paddingLarge
                        font.pixelSize: Theme.fontSizeMedium
                        color: listItem.down ?
                                   (model.unread ? Theme.highlightColor : Theme.secondaryHighlightColor) :
                                   (model.unread ? Theme.primaryColor : Theme.secondaryColor)
                        text: title
                    }
                }

                Rectangle {
                    id: unreadbox
                    anchors.right: parent.right; anchors.rightMargin: Theme.paddingLarge
                    y: Theme.paddingSmall
                    width: unreadlabel.width + 3 * Theme.paddingSmall
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

                /*Rectangle {
                    anchors.fill: image
                    color: Theme.secondaryColor
                    opacity: 0.1
                }*/

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
                        if (iconUrl=="") {
                            image.source = "";
                            return;
                        }
                        image.source = cache.getUrlbyUrl(iconUrl);
                    }
                }

                Component.onCompleted: {
                    if (iconUrl=="") {
                        image.source = "";
                        return;
                    }
                    image.source = cache.getUrlbyUrl(iconUrl);
                }

                onClicked: {
                    if (!listItem.last) {
                        if (settings.viewMode == 0) {
                            utils.setFeedModel(uid);
                            pageStack.push(Qt.resolvedUrl("FeedPage.qml"),{"title": title, "index": model.index});
                        }
                        if (settings.viewMode == 1) {
                            utils.setEntryModel(uid);
                            pageStack.push(Qt.resolvedUrl("EntryPage.qml"),{"title": title, "readlater": false});
                        }
                    }
                }

                showMenuOnPressAndHold: !listItem.last && model.unread+model.read>0

                menu: ContextMenu {
                    enabled: !listItem.last
                    MenuItem {
                        text: qsTr("Mark all as read")
                        enabled: model.unread!=0
                        visible: enabled
                        onClicked: {
                            tabModel.markAsRead(model.index);
                        }
                    }
                    MenuItem {
                        text: qsTr("Mark all as unread")
                        enabled: model.read!=0
                        visible: enabled
                        onClicked: {
                            tabModel.markAsUnread(model.index);
                        }
                    }
                }
            }
        }

        ViewPlaceholder {
            id: placeholder
            enabled: listView.count < 1
            text: fetcher.busy ? qsTr("Wait until Sync finish.") : qsTr("No tabs")
        }

        VerticalScrollDecorator {
            flickable: listView
        }
    }
}
