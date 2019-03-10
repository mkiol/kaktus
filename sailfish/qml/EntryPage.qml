/*
  Copyright (C) 2015-2019 Michal Kosciesza <michal@mkiol.net>

  This file is part of Kaktus.

  Kaktus is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Kaktus is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Kaktus.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: root

    objectName: "entries"

    allowedOrientations: {
        switch (settings.allowedOrientations) {
        case 1:
            return Orientation.Portrait;
        case 2:
            return Orientation.Landscape;
        }
        return Orientation.Landscape | Orientation.Portrait;
    }

    property bool showBar: true
    property alias remorse: _remorse
    property string title
    property bool landscapeMode: settings.doublePane && (app.isTablet || root.orientation === Orientation.Landscape)
    property EntryDelegate expandedDelegate
    property string expandedUid: ""
    property int expandedIndex: 0

    function openBrowser(index, link, uid) {
        entryModel.setData(index, "read", 1, "");
        notification.show(qsTr("Launching a browser..."));
        Qt.openUrlExternally(link);
    }

    function setContentPane(delegate) {
        //console.log("setContentPane",delegate);
        contentPanel.index = delegate.index
        //contentPanel.content = app.isTablet ? delegate.contentraw : delegate.contentall;
        contentPanel.content = delegate.contentall;
        //contentPanel.image = app.isTablet ? "" : delegate.image;
        contentPanel.image = delegate.image;
        contentPanel.expanded = false;
        delegate.expanded = true;
    }

    function getDelegateByUid(uid) {
        for (var i = 0; i < listView.contentItem.children.length; i++) {
            curItem = listView.contentItem.children[i];
            if (curItem.objectName === "EntryDelegate" && !curItem.last && !curItem.daterow &&
                    curItem.uid === uid) {
                return curItem;
            }
        }
        return undefined;
    }

    function clearContentPane(delegate) {
        if (delegate) {
            if (contentPanel.index === delegate.index) {
                contentPanel.index = 0;
                contentPanel.content = "";
                contentPanel.image = "";
                contentPanel.expanded = false;
            }
        } else {
            contentPanel.index = 0;
            contentPanel.content = "";
            contentPanel.image = "";
            contentPanel.expanded = false;
        }
    }

    function autoSetDelegate() {
        var delegate = root.expandedDelegate ? root.expandedDelegate : root.expandedUid!="" ? getDelegateByUid(root.expandedUid) : undefined;
        //console.log("autoSetDelegate",delegate);
        if (!delegate) {
            var curItem = listView.itemAt(0,listView.contentY + root.height/4);
            if (curItem.objectName === "EntryDelegate" && !curItem.last && !curItem.daterow) {
                curItem.expanded = true;
                //expandedDelegate = curItem;
                //expandedUid = curItem.uid;
                //expandedIndex = curItem.index;
                return;
            } else {
                for (var i = 0; i < listView.contentItem.children.length; i++) {
                    curItem = listView.contentItem.children[i];
                    if (curItem.objectName === "EntryDelegate" && !curItem.last && !curItem.daterow) {
                        curItem.expanded = true;
                        //expandedDelegate = curItem;
                        //expandedUid = curItem.uid;
                        //expandedIndex = curItem.index;
                        //listView.positionViewAtIndex(curItem.index, ListView.Contain);
                        return;
                    }
                }
            }
        }
    }

    onExpandedUidChanged: {
        //console.log("onExpandedUidChanged",root.expandedUid, root.expandedDelegate, root.expandedIndex);
        var delegate = root.expandedDelegate ? root.expandedDelegate : root.expandedUid!="" ? getDelegateByUid(root.expandedUid) : undefined;
        //console.log("delegate",delegate);
        if (delegate) {
            setContentPane(delegate);
        } else {
            clearContentPane(delegate);
        }
    }

    onStatusChanged: {
        if (status === PageStatus.Active) {
            if (landscapeMode)
                autoSetDelegate();
        }
    }

    onOrientationTransitionRunningChanged: {
        if (!orientationTransitionRunning) {
            //console.log("onOrientationTransitionRunningChanged");
            if (landscapeMode) {
                autoSetDelegate();
            } else {
                var delegate = root.expandedDelegate ? root.expandedDelegate : root.expandedUid!="" ? getDelegateByUid(root.expandedUid) : undefined;
                if (delegate)
                    listView.positionViewAtIndex(delegate.index, ListView.Contain);
            }
        }
    }

    ActiveDetector {
        onInit: { bar.flick = listView }
    }

    RemorsePopup {
        id: _remorse
    }

    Rectangle {
        anchors.fill: parent
        color: Theme.rgba(Theme.highlightDimmerColor, 0.2)
    }

    SilicaListView {
        id: listView
        model: entryModel

        anchors { top: parent.top; left: parent.left }
        width: root.landscapeMode && listView.count != 0 ?
                   parent.width - app.landscapeContentPanelWidth : parent.width
        clip: true
        height: app.flickHeight
        Behavior on height {NumberAnimation { duration: 200; easing.type: Easing.OutQuad }}

        onContentYChanged: {
            if (root.landscapeMode) {
                var itemTop = itemAt(0,contentY + root.height/5)
                var itemBottom = itemAt(0,contentY + 4*root.height/5)
                if (!itemTop.last && !itemTop.daterow) {
                    if (root.expandedDelegate) {
                        if (root.expandedDelegate.index < itemTop.index ||
                            root.expandedDelegate.index > itemBottom.index  )
                            itemTop.expanded = true
                        else
                            return
                    } else {
                        itemTop.expanded = true
                    }
                }
            }
        }

        PageMenu {
            id: menu
            showAbout: true
        }

        header: PageHeader {
            title: {
                switch (settings.viewMode) {
                case 3:
                    return qsTr("All feeds")
                case 4:
                    return app.isNetvibes ? qsTr("Saved") : qsTr("Starred")
                case 5:
                    return qsTr("Slow")
                case 6:
                    return qsTr("Liked")
                case 7:
                    return qsTr("Shared")
                default:
                    return root.title
                }
            }
        }

        delegate: EntryDelegate {
            id: delegate
            uid: model.uid
            title: model.title
            content: model.content
            contentall: model.contentall
            contentraw: model.contentraw
            date: model.date
            read: model.read
            friendStream: model.feedId.substring(0,4) === "user"
            feedIcon: model.feedIcon
            feedTitle: model.feedTitle
            author: model.author
            image: model.image
            readlater: model.readlater
            index: model.index
            cached: model.cached
            broadcast: model.broadcast
            liked: model.liked
            annotations: model.annotations
            fresh: model.fresh
            last: model.uid === "last"
            daterow: model.uid === "daterow"
            showMarkedAsRead: settings.viewMode!=4 && settings.viewMode!=6 && settings.viewMode!=7 && model.read<2
            objectName: "EntryDelegate"
            landscapeMode: root.landscapeMode
            onlineurl: model.link
            offlineurl: cserver.getUrlbyId(model.uid)
            evaluation: ai.evaluation(model.uid)

            signal singleEntryClicked
            signal doubleEntryClicked

            function check() {
                // Not allowed while Syncing
                if (dm.busy || fetcher.busy || dm.removerBusy) {
                    notification.show(qsTr("Wait until current task is complete."));
                    return false
                }

                // Entry not cached and offline mode enabled
                if (settings.offlineMode && !model.cached) {
                    notification.show(qsTr("Offline version is not available."));
                    return false
                }

                // Switch to offline mode if no network
                if (!settings.offlineMode && !dm.online) {
                    if (model.cached) {
                        // Entry cached
                        notification.show(qsTr("Enabling offline mode because network is disconnected."));
                        settings.offlineMode = true;
                    } else {
                        // Entry not cached
                        notification.show(qsTr("Network is disconnected."));
                        return false
                    }
                }

                return true
            }

            function openEntryInViewer() {
                pageStack.push(Qt.resolvedUrl("WebPreviewPage.qml"),
                               {"entryId": model.uid,
                                   "onlineUrl": delegate.onlineurl,
                                   "offlineUrl": delegate.offlineurl,
                                   "title": model.title,
                                   "stared": model.readlater===1,
                                   "liked": model.liked,
                                   "broadcast": model.broadcast,
                                   "index": model.index,
                                   "feedindex": root.index,
                                   "read" : model.read===1,
                                   "cached" : model.cached
                               })
            }

            function showEntryFeedContent() {
                // Not allowed while Syncing
                if (dm.busy || fetcher.busy || dm.removerBusy) {
                    notification.show(qsTr("Wait until current task is complete."))
                    return false
                }

                pageStack.push(Qt.resolvedUrl("FeedWebContentPage.qml"),
                               {"entryId": model.uid,
                                   "content": model.contentraw,
                                   "onlineUrl": delegate.onlineurl,
                                   "offlineUrl": delegate.offlineurl,
                                   "title": model.title,
                                   "stared": model.readlater===1,
                                   "liked": model.liked,
                                   "broadcast": model.broadcast,
                                   "index": model.index,
                                   "feedindex": root.index,
                                   "read" : model.read===1,
                                   "cached" : model.cached
                               })
            }

            function openEntry() {
                if (settings.clickBehavior === 2) {
                    showEntryFeedContent();
                    return
                }

                if (!check()) {
                    return
                }

                if (settings.clickBehavior === 1) {
                    openBrowser(model.index, model.link, model.uid);
                    return
                }

                openEntryInViewer()
            }

            Component.onCompleted: {
                //Dynamic creation of new items if last item is compleated
                if (index==entryModel.count()-2) {
                    entryModel.createItems(index+2,settings.offsetLimit)
                }
            }

            onClicked: {
                if (timer.running) {
                    // Double click
                    timer.stop()
                    doubleEntryClicked()
                } else {
                    timer.start()
                }
            }

            onDoubleEntryClicked: {
                if (model.read === 0) {
                    entryModel.setData(model.index, "read", 1, "")
                    //read = 1;
                } else {
                    entryModel.setData(model.index, "read", 0, "")
                    //read = 0;
                }
            }

            onSingleEntryClicked: {
                // Landscape mode
                if (root.landscapeMode) {
                    delegate.expanded = true
                    return
                }

                // Portrait mode
                openEntry()
            }

            Timer {
                id: timer
                interval: 400
                onTriggered: {
                    // Single click
                    singleEntryClicked()
                }
            }

            onExpandedChanged: {
                if (expanded) {
                    // Collapsing all other items on expand
                    for (var i = 0; i < listView.contentItem.children.length; i++) {
                        var curItem = listView.contentItem.children[i]
                        if (curItem !== delegate) {
                            if (curItem.objectName==="EntryDelegate") {
                                if (curItem.expanded)
                                    curItem.expanded = false
                            }
                        }
                    }

                    root.expandedDelegate = delegate
                    root.expandedUid = delegate.uid
                    root.expandedIndex = delegate.index
                } else {
                    if (delegate === root.expandedDelegate) {
                        root.expandedDelegate = null
                        root.expandedUid = ""
                        root.expandedIndex = 0
                    }
                }
            }

            onMarkedAsRead: {
                entryModel.setData(model.index, "read", 1, "")
            }

            onMarkedAsUnread: {
                entryModel.setData(model.index, "read", 0, "")
            }

            onMarkedReadlater: {
                entryModel.setData(index, "readlater", 1, "")
                if (evaluation !== -1) {
                    evaluation = 1
                    ai.addEvaluation(model.uid, model.title, evaluation)
                }
            }

            onUnmarkedReadlater: {
                entryModel.setData(index, "readlater", 0, "")
            }

            onMarkedLike: {
                entryModel.setData(model.index, "liked", true, "")
            }

            onUnmarkedLike: {
                entryModel.setData(model.index, "liked", false, "")
            }

            onMarkedBroadcast: {
                pageStack.push(Qt.resolvedUrl("ShareDialog.qml"),{"index": model.index,})
            }

            onUnmarkedBroadcast: {
                entryModel.setData(model.index, "broadcast", false, "")
            }

            onMarkedAboveAsRead: {
                entryModel.setAboveAsRead(model.index)
            }

            onShowFeedContent: {
                showEntryFeedContent()
            }

            onOpenInBrowser: {
                if (!check()) {
                    return
                }

                openBrowser(model.index, model.link, model.uid)
            }

            onOpenInViewer: {
                if (!check()) {
                    return
                }

                openEntryInViewer()
            }

            onEvaluated: {
                ai.addEvaluation(model.uid, model.title, evaluation)
            }

            onShare: {
                pageStack.push(Qt.resolvedUrl("ShareLinkPage.qml"),{"link": model.link, "linkTitle": model.title})
            }

            onPocketAdd: {
                pocket.add(model.link, model.title)
            }
        }

        ViewPlaceholder {
            id: placeholder
            enabled: listView.count === 0
            text: fetcher.busy ? qsTr("Wait until sync finish") :
                      settings.viewMode === 4 ? app.isNetvibes ? qsTr("No saved items") : qsTr("No starred items")  :
                      settings.viewMode === 6 ? qsTr("No liked items") : settings.showOnlyUnread ? qsTr("No unread items") : qsTr("No items")
        }

        VerticalScrollDecorator {
            flickable: listView
        }
    }

    EntryPageContent {
        id: contentPanel
        property bool expanded: false

        visible: root.landscapeMode && active
        anchors.right: root.right; anchors.top: root.top
        width: expanded ? root.width : app.landscapeContentPanelWidth
        clip: true
        height: app.flickHeight
        openable: false
        textFormat: app.isTablet ? Text.StyledText : Text.PlainText

        onClicked: {
            var delegate = root.expandedDelegate ?
                        root.expandedDelegate : root.expandedUid !="" ?
                            getDelegateByUid(root.expandedUid) : undefined
            if (delegate)
                delegate.openEntry()
        }

        onOpenClicked: {
            var delegate = root.expandedDelegate ?
                        root.expandedDelegate : root.expandedUid !="" ?
                            getDelegateByUid(root.expandedUid) : undefined
            if (delegate)
                delegate.openEntry()
        }

        busy: (width != root.width) &&
              (width !== app.landscapeContentPanelWidth)
    }

    HintLabel {
        anchors.bottom: parent.bottom
        backgroundColor: Theme.highlightDimmerColor
        Behavior on opacity { FadeAnimation { duration: 400 } }
        opacity: bar.open || settings.getHint1Done() ? 0.0 : 1.0
        visible: opacity != 0

        text: qsTr("One-tap to open article, double-tap to mark as read")

        MouseArea {
            anchors.fill: parent
            onPressed: {
                settings.setHint1Done(true);
                parent.opacity = 0.0;
            }
        }
    }
}
