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

import bb.cascades 1.3

Page {
    
    property bool menuEnabled: false
    
    Container {
        layout: DockLayout {}
        
        ViewPlaceholder {
            text: settings.signedIn ? fetcher.busy || dm.busy ? qsTr("You are signed in!\nWait until Sync finish.") : qsTr("To do feeds synchronisation, pull down and select Sync.") : qsTr("Not signed in to any account")
        }
        
        ProgressBar {
            verticalAlignment: VerticalAlignment.Bottom
        }
    }
    
    actions: [
        ActionItem {
            id: action1
            title: qsTr("Add account")
            enabled: !settings.signedIn
            imageSource: "asset:///add.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                fetcher.update();
            }
        }
    ]
}
