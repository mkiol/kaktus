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
import bb.system 1.0

SystemDialog {
    id: root
    
    signal ok
    signal cancel
    
    title: qsTr("Marking as unread")
    body: settings.viewMode == 3 
        ? qsTr("Mark all your articles as unread?")
        : settings.viewMode == 4 ? qsTr("Mark all saved articles as unread?")
        : qsTr("Mark articles as unread?")
        
    onFinished: {
        if (value==SystemUiResult.ConfirmButtonSelection) {
            ok();
            return;
        }
        cancel();
    }
}