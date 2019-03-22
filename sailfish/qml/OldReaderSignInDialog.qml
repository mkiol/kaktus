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

    canAccept: user.text.length > 0 && password.text.length > 0

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
        anchors {left: parent.left; right: parent.right; top: parent.top }
        height: app.flickHeight
        Behavior on height {NumberAnimation { duration: 200; easing.type: Easing.OutQuad }}
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
                acceptText : qsTr("Sign in")
            }

            Row {
                anchors { right: parent.right; rightMargin: Theme.horizontalPageMargin}
                spacing: Theme.paddingMedium
                height: Math.max(icon.height, label.height)

                Image {
                    id: icon
                    anchors.verticalCenter: parent.verticalCenter
                    source: "image://icons/icon-m-oldreader"
                    width: Theme.iconSizeMedium
                    height: Theme.iconSizeMedium
                }

                Label {
                    id: label
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Old Reader"
                    color: Theme.highlightColor
                    font.pixelSize: Theme.fontSizeSmall
                }
            }

            Spacer {}

            TextField {
                id: user
                anchors.left: parent.left; anchors.right: parent.right

                inputMethodHints: Qt.ImhEmailCharactersOnly| Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                placeholderText: qsTr("Enter username")
                label: qsTr("Username")

                Component.onCompleted: {
                    text = settings.getUsername();
                }

                EnterKey.iconSource: "image://theme/icon-m-enter-close"
                EnterKey.onClicked: {
                    Qt.inputMethod.hide();
                }
            }

            TextField {
                id: password
                anchors.left: parent.left; anchors.right: parent.right
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText | Qt.ImhSensitiveData
                echoMode: TextInput.Password
                placeholderText: qsTr("Enter password")
                label: qsTr("Password")

                EnterKey.iconSource: user.text.length > 0 ? "image://theme/icon-m-enter-accept" : "image://theme/icon-m-enter-close"
                EnterKey.onClicked: {
                    Qt.inputMethod.hide();
                    if (user.text.length > 0)
                        root.accept();
                }
            }

            Spacer {}
        }
    }

    onAccepted: {
        settings.setUsername(user.text);
        settings.setPassword(password.text);

        m.doInit = settings.signinType != 10;
        settings.signinType = 10;

        if (code == 0) {
            fetcher.checkCredentials();
        } else {
            if (!dm.busy)
                dm.cancel();
            m.doUpdate = true;
        }
    }

    // trick!
    QtObject {
        id: m
        property bool doUpdate: false
        property bool doInit: false
    }
    Component.onDestruction: {
        if (m.doUpdate) {
            if (m.doInit)
                fetcher.init();
            else
                fetcher.update();
        }
    }
}
