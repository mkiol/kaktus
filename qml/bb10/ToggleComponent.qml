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

Container {
    id: root

    property alias text: nameLabel.text
    property alias description: descriptionLabel.text
    property alias checked: toggle.checked
    property alias iconSource: image.imageSource

    topPadding: utils.du(1)
    bottomPadding: topPadding

    Container {

        layout: StackLayout {
            orientation: LayoutOrientation.LeftToRight
        }
        
        Label {
            id: nameLabel
            
            layoutProperties: StackLayoutProperties {
                spaceQuota: 1
            }
            
            verticalAlignment: VerticalAlignment.Center
        }
        
        ImageView {
            id: image
            visible: imageSource != ""
        }

        ToggleButton {
            id: toggle
            
            verticalAlignment: VerticalAlignment.Center
        }
    }
    
    Container {
        topMargin: utils.du(1)
        visible: root.description != ""
        
        Label {
            id: descriptionLabel
            multiline: true
            textStyle.base: SystemDefaults.TextStyles.SubtitleText
            textStyle.color: utils.secondaryText()
        }
        
    }
    
    
}