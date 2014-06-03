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

import QtQuick 1.1
import com.nokia.meego 1.0

import "Theme.js" as Theme

Page {
    id: root

    property int code

    tools: MainToolbar {}

    PageHeader {
        id: header
        title: qsTr("Netvibes account")
    }

    ListView {
        id: listView

        anchors {
            top: header.bottom; bottom: parent.bottom
            left: parent.left; right: parent.right
            margins: Theme.paddingMedium
        }

        spacing: Theme.paddingLarge

        model: VisualItemModel {

            Label {
                text: qsTr("Username")
            }

            TextField {
                id: user
                anchors.left: parent.left; anchors.right: parent.right

                inputMethodHints: Qt.ImhEmailCharactersOnly| Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                placeholderText: qsTr("Enter username here!")

                Component.onCompleted: {
                    text = settings.getNetvibesUsername();
                }
            }

            Label {
                text: qsTr("Password")
            }

            TextField {
                id: password
                anchors.left: parent.left; anchors.right: parent.right
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText | Qt.ImhSensitiveData
                echoMode: TextInput.Password
                placeholderText: qsTr("Enter password here!")
            }
        }
    }

    ScrollDecorator { flickableItem: listView }

    Button{
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.margins: UiConstants.DefaultMargin

        text: qsTr("Sign in")
        onClicked: accept()
    }

    function accept() {
        settings.setNetvibesUsername(user.text);
        settings.setNetvibesPassword(password.text);

        if (code == 0) {
            fetcher.checkCredentials();
        } else {
            if (!dm.busy)
                dm.cancel();
            //fetcher.update();
            m.doUpdate = true;
        }

        pageStack.pop();
    }

    // trick!
    QtObject {
        id: m
        property bool doUpdate: false
    }
    Component.onDestruction: {
        if (m.doUpdate)
            fetcher.update();
    }

}
