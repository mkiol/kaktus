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

DropDown {
    title: qsTr("View mode")
    
    property bool dark: Application.themeSupport.theme.colorTheme.style==VisualStyle.Dark
    
    options: [
        Option {
            selected: settings.viewMode == value
            value: 0
            imageSource: dark ? "asset:///vm0.png" : "asset:///vm0d.png"
            text: qsTr("Tabs & feeds")
            description: qsTr("All your tabs, feeds & articles")
        },
        Option {
            selected: settings.viewMode == value
            value: 1
            imageSource: dark ? "asset:///vm1.png" : "asset:///vm1d.png"
            text: qsTr("Only tabs")
            description: qsTr("All tabs & articles")
        },
        Option {
            selected: settings.viewMode == value
            value: 3
            imageSource: dark ? "asset:///vm3.png" : "asset:///vm3d.png"
            text: qsTr("All feeds")
            description: qsTr("All your articles")
        },
        Option {
            selected: settings.viewMode == value
            value: 4
            imageSource: dark ? "asset:///vm4.png" : "asset:///vm4d.png"
            text: qsTr("Saved")
            description: qsTr("Articles you have saved")
        },
        Option {
            selected: settings.viewMode == value
            value: 5
            imageSource: dark ? "asset:///vm5.png" : "asset:///vm5d.png"
            text: qsTr("Slow")
            description: qsTr("Less frequently updated feeds")
        }
    ]
    onSelectedOptionChanged: {
        settings.viewMode = selectedOption.value;
    }

}
