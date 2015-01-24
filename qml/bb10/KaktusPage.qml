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
    
    property bool menuEnabled: true
    property int modelType
    property variant model: modelType==0 ? tabModel : modelType==1 ? feedModel : modelType==2 ? entryModel : null
    
    property ActionMarkRead markAllAsReadAction
    property ActionMarkUnread markAllAsUnreadAction

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
                    }
            }

        }
    }
    
    property variant viewModeActions: []
    
    attachedObjects: [
        ReadAllDialog {
            id: readAllDialog
            onOk: {
                model.setAllAsRead();
            }
        },
        UnreadAllDialog {
            id: unreadAllDialog
            onOk: {
                model.setAllAsUnread();
            }
        }
    ]
    
    onCreationCompleted: {
        //createActions();
        refreshActions();
    }
    
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
    
    function refreshActions() {
        while (actions.length > 0) {
            removeAction(actions[0]);
        }
        
        /*var history = settings.viewModeHistory();
        for (var i in viewModeActions) {
            var inHistory = false;
            for (var ii in history) {
                if (viewModeActions[i].viewMode==history[ii]) {
                    inHistory = true;
                    break;
                }
            }
            addAction(viewModeActions[i], inHistory ? ActionBarPlacement.OnBar : ActionBarPlacement.Default);
        }*/

        // Mark as read is disabled for Saved view
        if (settings.viewMode != 4) {
            var read = model.countRead();
            var unread = model.countUnread();
            console.log("refreshActions",read,unread,root,modelType,settings.viewMode,actions.length);
            if (unread != 0)
                addAction(markAllAsReadAction);
            if (read != 0)
                addAction(markAllAsUnreadAction);
        }
        console.log("refreshActions2",read,unread,root,modelType,settings.viewMode,actions.length);
    }
}
