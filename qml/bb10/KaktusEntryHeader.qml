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
import org.labsquare 1.0
import "const.js" as Theme

Container {
    id: root
    
    property alias title: titleLabel.text
    
    background: Qt.utils.background() //Qt.utils.plainBase()
    preferredWidth: Qt.app.width
    

    layout: DockLayout {}

    // Bottom line
    Container {
        verticalAlignment: VerticalAlignment.Bottom
        background: Qt.utils.plainBase()
        minHeight: 2
        maxHeight: minHeight
        minWidth: Qt.app.width
        maxWidth: minWidth
    }
    
    // Top line
    Container {
        verticalAlignment: VerticalAlignment.Top
        background: Qt.utils.plainBase()
        minHeight: 1
        maxHeight: minHeight
        minWidth: Qt.app.width
        maxWidth: minWidth
    }
    
    Container {
        leftPadding: Qt.utils.du(2)
        rightPadding: Qt.utils.du(2)
        topPadding: Qt.utils.du(1)
        bottomPadding: Qt.utils.du(1)
        verticalAlignment: VerticalAlignment.Top
        
        Label {
            id: titleLabel
            textStyle.base: SystemDefaults.TextStyles.SubtitleText
            textStyle.fontWeight: FontWeight.W500
            textStyle.color: Qt.utils.secondaryText()
        }
    }
}