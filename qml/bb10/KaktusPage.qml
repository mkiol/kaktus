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

Page {
    id: root
    
    property bool menuEnabled: true
    property int modelType
    property variant model: modelType==0 ? tabModel : modelType==1 ? feedModel : modelType==2 ? entryModel : null

    property ActionMarkRead markAllAsReadAction
    property ActionMarkUnread markAllAsUnreadAction
    
    property bool busy: fetcher.busy || dm.busy || dm.removerBusy
    
    signal needToResetModel()
    
    onCreationCompleted: {
        //settings.showOnlyUnreadChanged.connect(refreshActions);
        refreshActions();
    }
    
    /*function disconnectSignals() {
    }*/
    
    actionBarAutoHideBehavior: ActionBarAutoHideBehavior.HideOnScroll
    actionBarVisibility: ChromeVisibility.Overlay
    
    markAllAsReadAction: ActionMarkRead {
        onTriggered: {
            switch (modelType) {
                case 0:
                case 1:
                    readAllDialog.show();
                    break;
                case 2:
                    if (settings.viewMode == 1 || settings.viewMode == 3 || settings.viewMode == 4 || settings.viewMode == 5) {
                        readAllDialog.show();
                    } else {
                        model.setAllAsRead();
                        //refreshActions();
                    }
            }
        }
    }

    markAllAsUnreadAction: ActionMarkUnread {
        onTriggered: {
            switch (modelType) {
                case 0:
                case 1:
                    unreadAllDialog.show();
                    break;
                case 2:
                    if (settings.viewMode == 1 || settings.viewMode == 3 || settings.viewMode == 4 || settings.viewMode == 5) {
                        unreadAllDialog.show();
                    } else {
                        model.setAllAsUnread();
                        
                        if (settings.showOnlyUnread && modelType==2) {
                            needToResetModel();
                        }
                        //refreshActions();
                    }
            }

        }
    }
    
    attachedObjects: [
        /*ActionItem {
            id: guideAction
            title: qsTr("Guide")
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                guide.open();
            }
        },*/
        ReadAllDialog {
            id: readAllDialog
            onOk: {
                model.setAllAsRead();
                //refreshActions();
            }
        },
        UnreadAllDialog {
            id: unreadAllDialog
            onOk: {
                model.setAllAsUnread();
                //refreshActions();
            }
        }
    ]
    
    onActionMenuVisualStateChanged: {
        if (actionMenuVisualState==ActionMenuVisualState.AnimatingToVisibleFull) {
            refreshActions();
        }
    }
    
    /*function createActions() {
        removeAllActions();
        var list = new Array(0)
        var modes = [0,1,3,4,5];
        for (var i in modes) {
            var obj = actionViewMode.createObject();
            obj.viewMode = modes[i];
            list.push(obj);
        }
        viewModeActions = list;
    }*/
    
    /*function refreshActions() {
        while (actions.length > 0) {
            removeAction(actions[0]);
        }

        // Mark as read is disabled for Saved view
        if (settings.viewMode != 4) {
            var read = model.countRead();
            var unread = model.countUnread();
            //console.log("refreshActions",read,unread,root,modelType,settings.viewMode,actions.length);
            if (unread != 0)
                addAction(markAllAsReadAction);
            if (!settings.showOnlyUnread && read != 0)
                addAction(markAllAsUnreadAction);
        }
        //console.log("refreshActions2",read,unread,root,modelType,settings.viewMode,actions.length);
    }*/
    
    
    
    function refreshActions() {
        //console.log("refreshActions");
        while (actions.length > 0) {
            removeAction(actions[0]);
        }
        
        //addAction(guideAction);
        addAction(markAllAsReadAction);
        if (settings.signinType < 10)
            addAction(markAllAsUnreadAction);
        
        /*if (!settings.showOnlyUnread)
            addAction(markAllAsUnreadAction);*/

        if (settings.viewMode == 4) {
            markAllAsReadAction.enabled = false;
            markAllAsUnreadAction.enabled = false;
            return;
        }

        var read = model.countRead();
        var unread = model.countUnread();
        if (unread != 0) {
            markAllAsReadAction.enabled = true;
        } else {
            markAllAsReadAction.enabled = false;
        }
        if (read != 0) {
            markAllAsUnreadAction.enabled = true;
        } else {
            markAllAsUnreadAction.enabled = false;
        }
    }
}
