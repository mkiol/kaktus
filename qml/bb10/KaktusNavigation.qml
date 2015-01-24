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

NavigationPane {
    id: nav
    
    property string  progressPanelText
    property double progressPanelValue
    
    property string  progressPanelDmText
    property double  progressPanelDmValue
    
    property string  progressPanelRemoverText
    property double  progressPanelRemoverValue
    
    onPopTransitionEnded: {
        if (nav.top.menuEnabled)
            Application.menuEnabled = true;
        else 
            Application.menuEnabled = false;
        page.disconnectSignals();
        page.destroy();
    }
    
    onPushTransitionEnded: {
        //console.log("onPushTransitionEnded");
        if (nav.top.menuEnabled)
            Application.menuEnabled = true;
        else 
            Application.menuEnabled = false;
    }
    
    property Page tempPage
    onNavigateToTransitionEnded: {
        if (nav.top.menuEnabled)
            Application.menuEnabled = true;
        else 
            Application.menuEnabled = false;
            
        for (var i = pages.length-1; i >= 0; i--) {
            tempPage = pages[i];
            tempPage.disconnectSignals();
            tempPage.destroy();
        }
    }
    
    onCreationCompleted: {
        // Theme
        setTheme();
        
        Qt.fetcher = fetcher;
        Qt.cache = cache;
        Qt.dm = dm;
        Qt.display = display;
        Qt.utils = utils;
        
        db.error.connect(dbError);
        db.empty.connect(dbEmpty);
        db.notEmpty.connect(dbNotEmpty);
        
        settings.error.connect(settingsError);
        settings.dashboardInUseChanged.connect(resetView);
        settings.viewModeChanged.connect(resetView);
        settings.signedInChanged.connect(settingsSignedInChanged);
        settings.themeChanged.connect(setTheme);
        
        fetcher.ready.connect(fetcherReady);
        fetcher.networkNotAccessible.connect(fetcherNetworkNotAccessible);
        fetcher.error.connect(fetcherError);
        fetcher.errorCheckingCredentials.connect(fetcherErrorCheckingCredentials);
        fetcher.credentialsValid.connect(fetcherCredentialsValid);
        fetcher.progress.connect(fetcherProgress);
        fetcher.uploading.connect(fetcherUploading);
        fetcher.busyChanged.connect(fetcherBusyChanged);
        
        dm.progress.connect(dmProgress);
        dm.networkNotAccessible.connect(dmNetworkNotAccessible);
        dm.removerProgressChanged.connect(dmRemoverProgressChanged);

        db.init();
    }
    
    function setTheme() {
        //console.log("setTheme",settings.theme,Application.themeSupport.theme.colorTheme.style);
        if (settings.theme == 0) {
            return;
        }
        Application.themeSupport.setVisualStyle(settings.theme);
    }
    
    function dbError(code) {
        console.log("DB error! code="+code);
        
        if (code===511) {
            notification.show(qsTr("Something went wrong :-(\nRestart the app to rebuild cache data."));
            return;
        }
        
        Qt.quit();
    }
    
    function dbEmpty() {
        console.log("DB empty!");
        
        dm.removeCache();
        if (settings.viewMode!=0)
            settings.viewMode=0;
        else
            resetView();
    }
    
    function dbNotEmpty() {
        resetView();
    }
    
    function settingsError(code) {
        console.log("Settings error!");
        console.log("code=" + code);
        Qt.quit();
    }
    
    function settingsSignedInChanged() {
        if (!settings.signedIn) {
            notification.show(qsTr("Signed out!"));
            fetcher.cancel(); dm.cancel();
            settings.reset();
            db.init();
        }
    }
    
    function dmProgress(remaining) {
        //console.log("DM progress: " + remaining);
        progressPanelDmText = qsTr("%1 more items left...").arg(remaining);
        if (remaining === 0) {
            progressPanelDmText = qsTr("All done!");
        }
    }
    
    function dmNetworkNotAccessible() {
        notification.show(qsTr("Download failed!\nNetwork connection is unavailable."));
    }
    
    function dmRemoverProgressChanged(current, total) {
        progressPanelRemoverValue = current / total;
    }
    
    function fetcherReady() {
        resetView();
        if (settings.autoDownloadOnUpdate) {
            dm.startFeedDownload();
        }
    }
    
    function fetcherNetworkNotAccessible() {
        notification.show(qsTr("Sync failed!\nNetwork connection is unavailable."));
    }
    
    function fetcherError(code) {
        console.log("Fetcher error");
        console.log("code=" + code);
        
        if (code < 400)
            return;
        if (code >= 400 && code < 500) {
            if (code == 402)
                notification.show(qsTr("The user name or password is incorrect!"));
            // Sign in
            var obj = signInDialog.createObject(); obj.code = code; nav.push(obj);
        } else {
            // Unknown error
            notification.show(qsTr("An unknown error occurred! :-("));
        }
    }
    
    function fetcherErrorCheckingCredentials() {
        notification.show(qsTr("The user name or password is incorrect!"));
    }
    
    function fetcherCredentialsValid() {
        notification.show(qsTr("You are signed in!"));
    }
    
    function fetcherProgress(current, total) {
        //console.log("Fetcher progress:", current/total);
        progressPanelText = qsTr("Receiving data... ");
        progressPanelValue = current / total;
    }
    
    function fetcherUploading() {
        progressPanelText = qsTr("Sending data...");
    }
    
    function fetcherBusyChanged() {
        switch(fetcher.busyType) {
            case 1:
                progressPanelText = qsTr("Initiating...");
                progressPanelValue = 0;
                break;
            case 2:
                progressPanelText = qsTr("Updating...");
                progressPanelValue = 0;
                break;
            case 3:
                progressPanelText = qsTr("Signing in...");
                progressPanelValue = 0;
                break;
            case 11:
                progressPanelText = qsTr("Waiting for network...");
                progressPanelValue = 0;
                break;
            case 21:
                progressPanelText = qsTr("Waiting for network...");
                progressPanelValue = 0;
                break;
            case 31:
                progressPanelText = qsTr("Waiting for network...");
                progressPanelValue = 0;
                break;
        }
    }
    
    function resetView() {
        if (!settings.signedIn) {
            Application.menuEnabled = false;
            nav.insert(0, firstPage.createObject());
            nav.navigateTo(nav.at(0)); 
            return;
        }
        
        utils.setRootModel();
        switch (settings.viewMode) {
            case 0:
            case 1:
                nav.insert(0, tabPage.createObject());
                nav.navigateTo(nav.at(0));
                break;
            case 2:
                nav.insert(0, feedPage.createObject());
                nav.navigateTo(nav.at(0));
                break;
            case 3:
            case 4:
            case 5:
                nav.insert(0, entryPage.createObject());
                nav.navigateTo(nav.at(0));
                break;
        }
    }

    attachedObjects: [
        ComponentDefinition {
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
        }
    ]
}