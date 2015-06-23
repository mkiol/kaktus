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

NavigationPane {
    id: nav
    
    property string  progressPanelText
    property double progressPanelValue
    
    property string  progressPanelDmText
    property double  progressPanelDmValue
    
    property string  progressPanelRemoverText
    property double  progressPanelRemoverValue
    
    property bool fetcherBusyStatus: false
    
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
        
        Qt.cache = cache;
        Qt.dm = dm;
        Qt.display = display;
        Qt.utils = utils;
        Qt.nav = nav;
        Qt.settings = settings;
        
        db.error.connect(dbError);
        db.empty.connect(dbEmpty);
        db.notEmpty.connect(dbNotEmpty);
        
        settings.error.connect(settingsError);
        settings.dashboardInUseChanged.connect(resetView);
        settings.viewModeChanged.connect(resetView);
        settings.signedInChanged.connect(settingsSignedInChanged);
        
        settings.themeChanged.connect(setTheme);
        
        dm.progress.connect(dmProgress);
        dm.networkNotAccessible.connect(dmNetworkNotAccessible);
        dm.removerProgressChanged.connect(dmRemoverProgressChanged);
        dm.busyChanged.connect(dmBusyChanged);
        dm.busyChanged.connect(dmBusyChanged);
        dm.removerBusyChanged.connect(dmRemoverBusyChanged);

        db.init();
    }
    
    function setTheme() {
        if (settings.theme == 0) {
            return;
        }
        
        // OS 10.3
        if (utils.checkOSVersion(10,3)) {
            Application.themeSupport.setVisualStyle(settings.theme);
        }
    }
    
    function reconnectFetcher(type) {
        disconnectFetcher();
        utils.resetFetcher(type);
        connectFetcher();
    }
    
    function connectFetcher() {
        if (typeof fetcher === 'undefined')
            return;
        fetcher.ready.connect(fetcherReady);
        fetcher.networkNotAccessible.connect(fetcherNetworkNotAccessible);
        fetcher.error.connect(fetcherError);
        fetcher.errorCheckingCredentials.connect(fetcherErrorCheckingCredentials);
        fetcher.credentialsValid.connect(fetcherCredentialsValid);
        fetcher.progress.connect(fetcherProgress);
        fetcher.uploading.connect(fetcherUploading);
        fetcher.busyChanged.connect(fetcherBusyChanged);
        fetcher.newAuthUrl.connect(fetcherNewAuthUrl);
        fetcher.errorGettingAuthUrl.connect(fetcherErrorGettingAuthUrl);
    }
    
    function disconnectFetcher() {
        if (typeof fetcher === 'undefined')
            return;
        fetcher.ready.disconnect(fetcherReady);
        fetcher.networkNotAccessible.disconnect(fetcherNetworkNotAccessible);
        fetcher.error.disconnect(fetcherError);
        fetcher.errorCheckingCredentials.disconnect(fetcherErrorCheckingCredentials);
        fetcher.credentialsValid.disconnect(fetcherCredentialsValid);
        fetcher.progress.disconnect(fetcherProgress);
        fetcher.uploading.disconnect(fetcherUploading);
        fetcher.busyChanged.disconnect(fetcherBusyChanged);
        fetcher.newAuthUrl.disconnect(fetcherNewAuthUrl);
        fetcher.errorGettingAuthUrl.disconnect(fetcherErrorGettingAuthUrl);
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
    
    function dmRemoverBusyChanged() {
        if (dm.removerBusy) {
            removerProgressDialog.show();
        } else {
            removerProgressDialog.cancel();
        }
    }
    
    function dmBusyChanged() {
        if (dm.busy) {
            if (!fetcher.busy)
                progressDialog.show();
        } else {
            if (!fetcher.busy)
                progressDialog.cancel();
        }
    }
    
    function fetcherReady() {
        resetView();

        switch (settings.cachingMode) {
        case 0:
            return;
        case 1:
            if (dm.isWLANConnected()) {
                dm.startFeedDownload();
            }
            return;
        case 2:
            dm.startFeedDownload();
            return;
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
            if (code == 403) {
                notification.show(qsTr("Your login credentials have expired!"));
                if (settings.getSigninType()>0) {
                    fetcher.getAuthUrl();
                    return;
                }
            }
            // Sign in
            var type = settings.signinType;
            if (type < 10) {
                var obj = nvSignInDialog.createObject(); obj.code = code; nav.push(obj);
                return;
            }
            if (type == 10) {
                var obj = oldReaderSignInDialog.createObject(); obj.code = code; nav.push(obj);
                return;
            }
            
            
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
    
    function fetcherErrorGettingAuthUrl() {
        notification.show(qsTr("Something goes wrong. Unable to sign in with Twitter! :-("));
    }
    
    function fetcherNewAuthUrl(url, type) {
        var obj = authWebViewPage.createObject();
        obj.url = url;
        obj.type = type;
        obj.code = 400;
        console.log("fetcherNewAuthUrl",url);
        nav.push(obj);
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
        if (fetcher.busy) {
            progressDialog.show();
        } else {
            if (!dm.busy)
                progressDialog.cancel();
        }
        
        if (nav.fetcherBusyStatus != fetcher.busy)
            nav.fetcherBusyStatus = fetcher.busy;
        
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
            case 4:
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
        
        // Reconnect fetcher
        if (typeof fetcher === 'undefined') {
            var type = settings.signinType;
            if (type < 10)
                reconnectFetcher(1);
            else if (type == 10)
                reconnectFetcher(2);
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
        ProgressDialog {
            id: progressDialog
            title: nav.fetcherBusyStatus ? qsTr("Synchronization") : dm.busy ? qsTr("Content caching...") : qsTr("Synchronization");
            progress: nav.fetcherBusyStatus ? progressPanelValue*100 : -1
            body: nav.fetcherBusyStatus ? progressPanelText : progressPanelDmText
            onCancelSelected: {
                //console.log("onCancelSelected", fetcher.busy, dm.busy);
                dm.cancel();
                fetcher.cancel();
                resetView();
            }
        },
        ProgressDialog {
            id: removerProgressDialog
            title: qsTr("Removing cache...")
            progress: progressPanelRemoverValue*100
            body: progressPanelRemoverText
        },
        ComponentDefinition {
            id: firstPage
            source: "FirstPage.qml"
        },
        ComponentDefinition {
            id: accountsDialog
            source: "AccountsDialog.qml"
        },
        ComponentDefinition {
            id: nvSignInDialog
            source: "NvSignInDialog.qml"
        },
        ComponentDefinition {
            id: oldReaderSignInDialog
            source: "OldReaderSignInDialog.qml"
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
        },
        ComponentDefinition {
            id: authWebViewPage
            source: "AuthWebViewPage.qml"
        }
    ]
}