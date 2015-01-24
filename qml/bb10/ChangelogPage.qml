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
    id: root
    
    titleBar: TitleBar {
        title: qsTr("Changelog")
    }
    
    Container {
        ScrollView {
            Container {
                Header {
                    title: qsTr("Version %1").arg("1.2.3")
                }

                Container {
                    leftPadding: ui.du(2)
                    rightPadding: ui.du(2)
                    topPadding: ui.du(2)
                    bottomPadding: ui.du(2)

                    LogItem {
                        title: 'Initial version for BlackBerry'
                        description: 'This is first version of Kaktus for BlackBerry 10.'
                    }
                }
            }
        }

    }
}
