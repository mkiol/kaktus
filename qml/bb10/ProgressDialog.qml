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
import bb.system 1.2

SystemProgressDialog {
    id: root
    
    property bool active: state==SystemUiProgressState.Active
    
    signal cancelSelected;
    
    autoUpdateEnabled: true
    confirmButton.label: qsTr("Cancel")
    dismissAutomatically: true
    
    onFinished: {
        //console.log("onFinished", value==SystemUiResult.ConfirmButtonSelection, value);
        if (value==SystemUiResult.ConfirmButtonSelection) {
            cancelSelected();
        }
    }
}
