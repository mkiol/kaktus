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

Container {
    id: root
    
    property alias text: label.text
    property alias value: value.text
    property alias buttonText: button.text
    signal clicked
    
    topPadding: ui.du(1)
    bottomPadding: topPadding
    
    layout: StackLayout {
        orientation: LayoutOrientation.LeftToRight
    }
    
    Label {
        id: label
        
        layoutProperties: StackLayoutProperties {
            spaceQuota: 1
        }
        verticalAlignment: VerticalAlignment.Center
        multiline: true
    }
    
    Label {
        id: value
        
        layoutProperties: StackLayoutProperties {
            spaceQuota: -1
        }
        visible: text != ""
        textStyle.color: ui.palette.primary
        verticalAlignment: VerticalAlignment.Center
        horizontalAlignment: HorizontalAlignment.Center
        multiline: true
    }
    
    Button {
        id: button
        
        layoutProperties: StackLayoutProperties {
            spaceQuota: 1
        }
        visible: text != ""
        verticalAlignment: VerticalAlignment.Center
        onClicked: {
            root.clicked();
        }
    }
}
