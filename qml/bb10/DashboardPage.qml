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
import com.kdab.components 1.0

KaktusPage {
    id: root
    
    property bool menuEnabled: false
    
    function disconnectSignals() {
        nav.topChanged.disconnect(update);
    }
    
    titleBar: TitleBar {
        title: qsTr("Dashboards")
    }
    
    attachedObjects: [
        AbstractItemModel {
            id: bbDashboardModel
            sourceModel: dashboardModel
        }
    ]
    
    onCreationCompleted: {
        Qt.dashboardModel = dashboardModel;
        nav.topChanged.connect(update);
    }
    
    Container {
        layout: DockLayout {}
        
        ListView {
            id: listView
            
            dataModel: bbDashboardModel
            verticalAlignment: VerticalAlignment.Top
            visible: dataModel.count!=0
            
            layout: StackListLayout {
                headerMode: ListHeaderMode.None
            }
            
            listItemComponents: [
                ListItemComponent {
                    type: ""
                    StandardListItem {
                        title: ListItemData.title
                    }
                }
            ]
            
            onTriggered: {
                var chosenItem = dataModel.data(indexPath);
                if (settings.dashboardInUse != chosenItem.uid)
                    settings.dashboardInUse = chosenItem.uid;
                else
                    nav.pop();
            }
            
            accessibility.name: "Dashboard list"
        }
        
        ViewPlaceholder {
            text: qsTr("No dashboards")
            visible: bbDashboardModel.count==0
        }
        
        ProgressBar {
        }
    }
}
