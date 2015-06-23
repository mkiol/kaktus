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
import "const.js" as Theme

Container {
    verticalAlignment: VerticalAlignment.Top
    horizontalAlignment: HorizontalAlignment.Left
    
    //background: Color.Red
       
    Container {
        background: utils.plainBase()
        preferredHeight: utils.du(0.25)
        preferredWidth: display.pixelSize.width
        visible: _progressPanel.open || _progressPanelDm.open || _progressPanelRemover.open
    }
    
    ProgressPanel {
        id: _progressPanelRemover
        open: dm.removerBusy && !_progressPanel.open && !_progressPanelDm
        text: nav.progressPanelRemoverText
        progress: nav.progressPanelRemoverValue
        onCancelClicked: dm.removerCancel();
    }
    
    ProgressPanel {
        id: _progressPanelDm
        open: dm.busy && !_progressPanel.open
        text: nav.progressPanelDmText
        //progress: nav.progressPanelDmValue
        showProgress: false
        onCancelClicked: dm.cancel();
    }
    
    ProgressPanel {
        id: _progressPanel
        open: fetcher.busy
        //open: true
        text: nav.progressPanelText
        progress: nav.progressPanelValue
        onCancelClicked: fetcher.cancel();
    }
}
