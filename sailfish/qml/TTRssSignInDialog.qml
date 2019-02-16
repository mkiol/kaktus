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

    canAccept: url.text != "" && user.text != "" && password.text != ""

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
        anchors {top: parent.top}
        height: app.flickHeight
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

            Item {
                anchors { left: parent.left; right: parent.right}
                height: Math.max(icon.height, label.height)

                Image {
                    id: icon
                    anchors { right: label.left; rightMargin: Theme.paddingMedium }
                    source: "ttrss.png"
                    width: Theme.iconSizeMedium
                    height: Theme.iconSizeMedium
                }

                Label {
                    id: label
                    anchors { right: parent.right; rightMargin: Theme.paddingLarge}
                    text: qsTr("Tiny Tiny Rss")
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

            PaddedLabel {
                text: qsTr("Enter server url and credentials below.")
            }

            Item {
                height: Theme.paddingMedium
                width: Theme.paddingMedium
            }

            TextField {
                id: url
                anchors.left:parent.left; anchors.right: parent.right

                inputMethodHints: Qt.ImhEmailCharactersOnly | Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                placeholderText: qsTr("Enter the url of your server here!")
                label: qsTr("Server Url")

                Component.onCompleted: {
                    text = settings.getUrl();
                }

                EnterKey.iconSource: "image://theme/icon-m-enter-close"
                EnterKey.onClicked: {
                    Qt.inputMethod.hide();
                }
            }

            TextField {
                id: user
                anchors.left: parent.left; anchors.right: parent.right

                inputMethodHints: Qt.ImhEmailCharactersOnly| Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                placeholderText: qsTr("Enter username here!")
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
                placeholderText: qsTr("Enter password here!")
                label: qsTr("Password")

                EnterKey.iconSource: url.text!=="" && user.text!=="" ? "image://theme/icon-m-enter-accept" : "image://theme/icon-m-enter-close"
                EnterKey.onClicked: {
                    Qt.inputMethod.hide();
                    if (url.text!=="" && user.text!=="")
                        root.accept();
                }
            }

            Item {
                height: Theme.itemSizeLarge
                width: Theme.itemSizeLarge
            }
        }
    }

    function validateEmail(email) {
        var re = /^(([^<>()[\]\\.,;:\s@\"]+(\.[^<>()[\]\\.,;:\s@\"]+)*)|(\".+\"))@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\])|(([a-zA-Z\-0-9]+\.)+[a-zA-Z]{2,}))$/;
        return re.test(email);
    }

    onAccepted: {
        settings.setUrl(url.text);
        settings.setUsername(user.text);
        settings.setPassword(password.text);

        m.doInit = settings.signinType != 30;
        settings.signinType = 30;

        if (code == 0) {
            fetcher.checkCredentials();
        } else {
            if (! dm.busy)
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
