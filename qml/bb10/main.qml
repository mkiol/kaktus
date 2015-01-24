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
import bb.device 1.3

TabbedPane {
    id: app
    
    showTabsOnActionBar: false
    
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
        Notification {
            id: notification
        },
        DisplayInfo {
            id: display
        },
        /*ComponentDefinition {
            id: firstPage
            source: "FirstPage.qml"
        },
        ComponentDefinition {
            id: signInDialog
            source: "SignInDialog.qml"
        },
        ComponentDefinition {
            id: tabPage
            source: "TabPage.qml"
        },
        ComponentDefinition {
            id: feedPage
            source: "FeedPage.qml"
        },
        ComponentDefinition {
            id: entryPage
            source: "EntryPage.qml"
        },*/
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
        },
        KaktusNavigation {
            id: nav
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
                onTriggered: {
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
    
    onCreationCompleted: {
        switch (settings.viewMode) {
        case 0:
            activeTab = tab0;
            break;
        case 1:
            activeTab = tab1;
            break;
        case 2:
            activeTab = tab2;
            break;
        case 3:
            activeTab = tab3;
            break;
        case 4:
            activeTab = tab4;
            break;
        case 5:
            activeTab = tab5;
            break;
        }
        
        // Workaround
        remove(nullTab)
    }
    
    Tab {
        id: nullTab
    }
    
    KaktusTab {
        id: tab0
        viewMode: 0
        content: nav
    }
    
    KaktusTab {
        id: tab1
        viewMode: 1
        content: nav
    }
    
    KaktusTab {
        id: tab2
        viewMode: 2
        content: nav
    }
    
    KaktusTab {
        id: tab3
        viewMode: 3
        content: nav
    } 
    
    KaktusTab {
        id: tab4
        viewMode: 4
        content: nav
    } 
    
    KaktusTab {
        id: tab5
        viewMode: 5
        content: nav
    }
}