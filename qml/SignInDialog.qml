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

    property int code

    canAccept: user.text !== "" && password.text !== ""

    Column {
        anchors {
            left: parent.left; leftMargin: Theme.paddingMedium
            right: parent.right; rightMargin: Theme.paddingMedium
        }
        spacing: Theme.paddingSmall

        DialogHeader {
            title: qsTr("Netvibes account")
            acceptText : qsTr("Sign In")
        }

        Rectangle {
            id: infoLabel
            anchors {
                left: parent.left; leftMargin: Theme.paddingLarge
                right: parent.right; rightMargin: Theme.paddingLarge
            }
            height: childrenRect.height + 2*Theme.paddingLarge
            color: Theme.rgba(Theme.highlightBackgroundColor, 0.2)
            radius: 5

            Label {
                anchors.centerIn:  parent;
                wrapMode: Text.Wrap
                textFormat: Text.PlainText
                font.pixelSize: Theme.fontSizeSmall

                Component.onCompleted: {
                    switch (code) {
                    case 402:
                        text = qsTr("Username & password do not match!");
                        break;
                    default:
                        text = qsTr("Enter Netvibes username & password");
                    }
                }
            }
        }

        Label {
            text: " "
        }

        /*Label {
            text: qsTr("Username")
        }*/

        TextField {
            id: user
            anchors.left: parent.left; anchors.right: parent.right

            inputMethodHints: Qt.ImhEmailCharactersOnly| Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
            placeholderText: "Enter username here!"
            label: qsTr("Username")

            Component.onCompleted: {
                text = settings.getNetvibesUsername();
            }
        }

        /*Label {
            text: qsTr("Password")
        }*/

        TextField {
            id: password
            anchors.left: parent.left; anchors.right: parent.right
            inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText | Qt.ImhSensitiveData
            echoMode: TextInput.Password
            placeholderText: "Enter password here!"
            label: qsTr("Password")
        }

    }

    onAccepted: {
        settings.setNetvibesUsername(user.text);
        settings.setNetvibesPassword(password.text);

        if (code == 0)
            fetcher.checkCredentials();
    }

}
