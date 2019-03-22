/*
  Copyright (C) 2015 Michal Kosciesza <michal@mkiol.net>

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
    property int index
    property bool actionDone: false

    canAccept: note.text.length > 0

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
        anchors.bottomMargin: app.height - app.flickHeight
        contentHeight: content.height

        Column {
            id: content
            anchors {
                left: parent.left
                right: parent.right
            }

            spacing: Theme.paddingSmall

            DialogHeader {
                //title: qsTr("Adding note")
                acceptText : qsTr("Save note")
            }

            TextArea {
                id: note
                anchors.left: parent.left; anchors.right: parent.right
                placeholderText: qsTr("Want to add a note?")
                label: qsTr("Note")
                focus: true
            }
        }
    }

    onAccepted: {
        entryModel.setData(index, "broadcast", true, note.text);
        actionDone = true;
    }

    Component.onDestruction: {
        if (!actionDone)
            entryModel.setData(index, "broadcast", true, "");
    }

}
