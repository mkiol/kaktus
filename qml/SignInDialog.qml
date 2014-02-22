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

    canAccept: user.text!=="" && password.text !==""

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

        TextField {
            id: user
            anchors.left: parent.left; anchors.right: parent.right

            inputMethodHints: Qt.ImhEmailCharactersOnly| Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
            placeholderText: qsTr("Enter username here!")
            label: qsTr("Netvibes's username")

            Component.onCompleted: {
                text = settings.getNetvibesUsername();
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

            EnterKey.iconSource: user.text!=="" ? "image://theme/icon-m-enter-accept" : "image://theme/icon-m-enter-close"
            EnterKey.onClicked: {
                Qt.inputMethod.hide();
                if (user.text!=="")
                    root.accept();
            }
        }
    }

    onAccepted: {
        settings.setNetvibesUsername(user.text);
        settings.setNetvibesPassword(password.text);

        if (code == 0)
            fetcher.checkCredentials();
    }

}
