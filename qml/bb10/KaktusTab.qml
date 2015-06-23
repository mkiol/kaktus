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

Tab {
    property int viewMode: 0
    
    title: viewMode==0 ? settings.signinType<10 ? qsTr("Tabs, feeds & articles") : qsTr("Folders, feeds & articles") :
    viewMode==1 ? settings.signinType<10 ? qsTr("Tabs & articles") : qsTr("Folders & articles") :
    viewMode==2 ? qsTr("All feeds") :
    viewMode==3 ? qsTr("All articles") :
    viewMode==4 ? settings.signinType<10 ? qsTr("Saved") : qsTr("Starred") :
    viewMode==5 ? utils.isLight() ? qsTr("Slow (only in pro edition)") : qsTr("Slow") : ""
    
    /*description: viewMode==0 ? qsTr("All your tabs, feeds & articles") :
    viewMode==1 ? qsTr("All tabs & articles") :
    viewMode==2 ? qsTr("Feeds & articles") :
    viewMode==3 ? qsTr("All your articles"):
    viewMode==4 ? qsTr("Articles you have saved") :
    viewMode==5 ? utils.isLight() ? qsTr("Less frequently updated feeds") : qsTr("Less frequently updated feeds") : ""*/
    
    imageSource: "asset:///vm"+viewMode+".png"
    
    enabled: utils.isLight() ? settings.signedIn && !fetcher.busy && viewMode!=5 : settings.signedIn && !fetcher.busy
    
    onTriggered: {
        settings.viewMode = viewMode;
    }
}
