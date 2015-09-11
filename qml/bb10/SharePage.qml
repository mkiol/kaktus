/*
 * Copyright (C) 2015 Michal Kosciesza <michal@mkiol.net>
 * 
 * This file is part of Kaktus.
 * 
 * Kaktus is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Kaktus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Kaktus.  If not, see <http://www.gnu.org/licenses/>.
 */

import bb.cascades 1.2

Page {
    id: root
    
    property int index
    
    property bool actionDone: false
    
    onCreationCompleted: {
        Qt.nav.popTransitionEnded.connect(popTransitionEnded);
    }

    function popTransitionEnded(page) {
        if (!actionDone) {
            console.log("popTransitionEnded");
            Qt.nav.popTransitionEnded.disconnect(popTransitionEnded);
            Qt.entryModel.setData(index, "broadcast", true, "");
            root.actionDone = true;
        }
    }

    titleBar: TitleBar {
        acceptAction: ActionItem {
            title: qsTr("Save")
            onTriggered: {
                Qt.entryModel.setData(index, "broadcast", true, textArea.text);
                root.actionDone = true;
                Qt.nav.pop();
            }
        }
        
        dismissAction: ActionItem {
            title: qsTr("Cancel")
            onTriggered: {
                Qt.entryModel.setData(index, "broadcast", true, "");
                root.actionDone = true;
                Qt.nav.pop();
            }
        }

        title: qsTr("Adding note")
        appearance: TitleBarAppearance.Plain
    }
    
    function disconnectSignals() {
    }

    TextArea {
        id: textArea
        hintText: "Want to add a note?"
        input.submitKey: SubmitKey.Submit
        preferredHeight: Qt.app.height
    }
    
}
