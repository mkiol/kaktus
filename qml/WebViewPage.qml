/*
  Copyright (C) 2014 Michal Kosciesza <michal@mkiol.net>

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
import QtWebKit 3.0
import QtWebKit.experimental 1.0

Page {
    id: root

    property string title
    property string entryId
    property string offlineUrl
    property string onlineUrl

    // work around Silica bug: don't let webview enable forward navigation
    onForwardNavigationChanged: {
        if (forwardNavigation)
            forwardNavigation = false;
    }

    allowedOrientations: Orientation.Landscape | Orientation.Portrait

    SilicaWebView {
        id: view

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        url:  offLineMode ? offlineUrl : onlineUrl
        experimental.userAgent: settings.getDmUserAgent()

        onUrlChanged: {
            console.log("onUrlChanged");
        }

        header: PageHeader {
            title: root.title
        }

        onLoadingChanged: {
            if (loadRequest.status == WebView.LoadStartedStatus) {
                console.log("onLoadingChanged, LoadStartedStatus");
            } else if (loadRequest.status == WebView.LoadFailedStatus) {
                console.log("onLoadingChanged, LoadFailedStatus");
            } else {
                console.log("onLoadingChanged, Ok");
            }
        }

        onNavigationRequested: {
            // In Off-Line mode navigation is disabled
            if (offLineMode) {
                if (request.url != offlineUrl) {
                    request.action = WebView.IgnoreRequest;
                }
            }
        }

        PullDownMenu {
            MenuItem {
                text: qsTr("Copy URL")

                onClicked: {
                    if (offLineMode)
                        utils.copyToClipboard(offlineUrl);
                    else
                        utils.copyToClipboard(onlineUrl);
                }
            }

            MenuItem {
                text: qsTr("Open in browser")

                onClicked: {
                    if (offLineMode)
                        Qt.openUrlExternally(offlineUrl);
                    else
                        Qt.openUrlExternally(onlineUrl);
                }
            }

            MenuItem {
                text: qsTr("Back")

                onClicked: {
                    pageStack.pop()
                }
            }
        }

        PushUpMenu {
            MenuItem {
                text: qsTr("Top")
                onClicked: view.scrollToTop()
            }
        }
    }

}
