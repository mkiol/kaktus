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
    property alias text: label.text
    property alias textStyle: label.textStyle
    property alias textFormat: label.textFormat
    
    topMargin: utils.du(2)
    bottomMargin: utils.du(2)
    leftPadding: utils.du(2)
    rightPadding: utils.du(2)
    
    Label {
        id: label
        multiline: true
    }
    
}
