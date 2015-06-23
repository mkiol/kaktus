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
import bb.device 1.2

TabbedPane {
    id: app
    
    showTabsOnActionBar: false
    
    onCreationCompleted: {
        settings.signedInChanged.connect(refreshTabs);
        refreshTabs();
    }
    
    onSidebarVisualStateChanged: {
        //console.log("onSidebarVisualStateChanged",sidebarVisualState);
        if (sidebarVisualState==SidebarVisualState.AnimatingToVisibleFull || 
            sidebarVisualState==SidebarVisualState.AnimatingToVisibleCompact) {
                if (!settings.signedIn || fetcher.busy) {
                    sidebarState = SidebarState.Hidden;
                }
        }
    }
    
    onSidebarStateChanged: {
        //console.log("onSidebarStateChanged",sidebarState);
        //sidebarState = SidebarState.VisibleCompact
    }

    attachedObjects: [
        KaktusNavigation {
            id: nav
        },
        Notification {
            id: notification
        },
        ComponentDefinition {
            id: kaktusTab
            source: "KaktusTab.qml"
        },
        ComponentDefinition {
            id: dashboardPage
            source: "DashboardPage.qml"
        },
        ComponentDefinition {
            id: settingsPage
            source: "SettingsPage.qml"
        },
        ComponentDefinition {
            id: webPreviewPage
            source: "WebPreviewPage.qml"
        },
        ComponentDefinition {
            id: actionViewMode
            source: "ActionViewMode.qml"
        },
        ComponentDefinition {
            id: aboutPage
            source: "AboutPage.qml"
        },
        ComponentDefinition {
            id: changelogPage
            source: "ChangelogPage.qml"
        }
    ]
    
    Menu.definition: MenuDefinition {
        
        settingsAction: SettingsActionItem {
            onTriggered: {
                nav.push(settingsPage.createObject());
            }
        }
        
        actions: [
            ActionSync {},
            ActionNetworkMode {
                enabled: !utils.isLight()
                onTriggered: {
                    if (utils.isLight()) {
                        notification.show(qsTr("Offline mode is available only in pro edition of Kaktus."));
                        return;
                    }
                    if (settings.offlineMode) {
                        if (dm.online)
                            settings.offlineMode = false;
                        else
                            notification.show(qsTr("Cannot switch to Online mode.\nNetwork connection is unavailable."));
                    } else {
                        settings.offlineMode = !settings.offlineMode;
                    }
                }
            }
        ]
        
        helpAction: HelpActionItem {
            title: qsTr("About")
            onTriggered: {
                nav.push(aboutPage.createObject());
            }
        }
    }
    
    function refreshTabs() {
        removeTabs();
        
        if (settings.signedIn) {
            addTab(0);
            addTab(1);
            addTab(3);
            addTab(4);
            addTab(5);
            setActiveTab();
        } else {
            activeTab = addTab(settings.viewMode);
        }
    }
    
    function removeTabs() {
        for (var i = tabs.length-1; i >= 0; i--) {
            remove(tabs[i]);
        }
    }
    
    function setActiveTab() {
        //console.log("setActiveTab: settings.viewMode",settings.viewMode,"activeTab",activeTab);
        switch (settings.viewMode) {
            case 0:
                activeTab = tabs[0];
                break;
            case 1:
                activeTab = tabs[1];
                break;
            case 3:
                activeTab = tabs[2];
                break;
            case 4:
                activeTab = tabs[3];
                break;
            case 5:
                activeTab = tabs[4];
                break;
            default :
                activeTab = tabs[0];
        }
    }
    
    function addTab(viewMode) {
        var tab = kaktusTab.createObject(app);
        tab.viewMode = viewMode;
        tab.content = nav;
        add(tab);
        return tab;
    }
}