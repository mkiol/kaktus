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

ActionItem {
    title: enabled ? qsTr("Sync") : qsTr("Syncing...")
    enabled: !nav.fetcherBusyStatus && !dm.busy && !dm.removerBusy
    ActionBar.placement: ActionBarPlacement.OnBar
    imageSource: "asset:///sync.png"
    onTriggered: {
        if (settings.signedIn) {
            fetcher.update();
        } else {
            nav.push(nav.signInDialog.createObject());
        }
    }
}