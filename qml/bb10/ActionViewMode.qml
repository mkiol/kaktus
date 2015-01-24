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

ActionItem {
    id: root
    property int viewMode: 0
    
    title: viewMode==0 ? qsTr("Tabs & Feeds") :
    viewMode==1 ? qsTr("Only Tabs") :
    viewMode==2 ? qsTr("Only feeds") :
    viewMode==3 ? qsTr("All feeds") :
    viewMode==4 ? qsTr("Saved") :
    viewMode==5 ? qsTr("Slow") : ""
    imageSource: settings.viewMode==viewMode ? "asset:///vm"+viewMode+"b.amd" : "asset:///vm"+viewMode+".png"
    //ActionBar.placement: settings.viewMode == viewMode ? ActionBarPlacement.OnBar : ActionBarPlacement.Default
    //enabled: settings.viewMode != viewMode
    onTriggered: {
        settings.viewMode = viewMode;
    }
}
