/*
  Copyright (C) 2017 Michal Kosciesza <michal@mkiol.net>

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

Item {
    id: root

    property bool busy: false

    readonly property string consumer_key: settings.pocketConsumerKey()
    readonly property string redirect_uri: "kaktus:authorizationFinished"
    readonly property string request_url: "https://getpocket.com/v3/oauth/request"
    readonly property string authorize_url: "https://getpocket.com/auth/authorize"
    readonly property string oauth_authorize_url: "https://getpocket.com/v3/oauth/authorize"
    readonly property string add_url: "https://getpocket.com/v3/add"

    property string request_token
    property string access_token: settings.pocketToken

    function _split(tags) {
        var _tags = tags.split(",")
        var tagsArr = []
        for (var i = 0; i < _tags.length; i++) {
            var tag = _tags[i].trim().toLowerCase()
            if (tag !== "") {
                if (tagsArr.indexOf(tag) === -1) {
                    tagsArr.push(tag)
                }
            }
        }
        return tagsArr
    }

    function fixTags(tags) {
        return _split(tags).join(", ")
    }

    function enable() {
        busy = true
        settings.pocketToken = ""
        _requestToken(function() {
            console.log("request token: " + root.request_token)
            pageStack.push(Qt.resolvedUrl("PocketAuthWebViewPage.qml"))
        }, function(code) {
            console.log("X-Error:" + xhr.getResponseHeader("X-Error"))
            console.log("Error while requesting Pocket token, X-Error-Code:" + code)
            notification.show(qsTr("Pocket authorization has failed."))
            busy = false
        })
    }

    function check() {
        _accessToken(function() {
            console.log("access token: " + root.access_token)
            settings.pocketToken = root.access_token
            settings.pocketEnabled = true
            notification.show(qsTr("Pocket authorization was successful."))
            busy = false
        }, function(code) {
            console.log("X-Error:" + xhr.getResponseHeader("X-Error"))
            console.log("Error while requesting Pocket access token, X-Error-Code:" + code)
            notification.show(qsTr("Pocket authorization has failed."))
            busy = false
        })
    }

    function cancel() {
        busy = false
    }

    function getAuthUrl() {
        return authorize_url + "?request_token=" + request_token + "&redirect_uri=" + redirect_uri
    }

    function add(url, title) {
        if (settings.pocketQuickAdd)
            quickAdd(url, title, settings.pocketTags)
        else
            pageStack.push(Qt.resolvedUrl("PocketDialog.qml"),{"url": url, "title": title})
    }

    function quickAdd(url, title, tags) {
        tags = tags == null ? "" : fixTags(tags)
        title = title == null ? "" : title.trim()
        var req = {
            consumer_key: consumer_key,
            access_token: access_token,
            url: url,
            title: title,
            tags: tags
        }
        var xhr = new XMLHttpRequest()
        xhr.open("POST", add_url)
        xhr.setRequestHeader("Content-Type", "application/json; charset=UTF8")
        xhr.setRequestHeader("X-Accept", "application/json")
        xhr.onreadystatechange = function () {
                if(xhr.readyState === XMLHttpRequest.DONE) {
                    busy = false
                    if (xhr.status === 200) {
                        notification.show(qsTr("Article has been successfully added to Pocket."))
                        settings.pocketTagsHistory = _split(settings.pocketTagsHistory + "," + tags).sort().join(",")
                    } else {
                        console.log("X-Error:" + xhr.getResponseHeader("X-Error"))
                        var code = xhr.getResponseHeader("X-Error-Code")
                        console.log("Error while adding article to Pocket, X-Error-Code:" + code)
                        notification.show(qsTr("Error while adding article to Pocket."))
                    }
                }
            }
        busy = true
        xhr.send(JSON.stringify(req))
    }

    // Private

    function _requestToken(ok, error) {
        var req = {
            consumer_key: consumer_key,
            redirect_uri: redirect_uri
        }
        var xhr = new XMLHttpRequest()
        xhr.open("POST", request_url)
        xhr.setRequestHeader("Content-Type", "application/json; charset=UTF8")
        xhr.setRequestHeader("X-Accept", "application/json")
        xhr.onreadystatechange = function () {
                /*console.log("xhr.onreadystatechange")
                console.log("  xhr.readyState: " + xhr.readyState)
                console.log("  xhr.status: " + xhr.status)
                console.log("  xhr.responseType: " + xhr.responseType)
                console.log("  xhr.responseURL : " + xhr.responseURL )
                console.log("  xhr.statusText: " + xhr.statusText)
                console.log("  xhr.responseText: " + xhr.responseText)
                console.log("  X-Error-Code:" + xhr.getResponseHeader("X-Error-Code"))*/
                if(xhr.readyState === XMLHttpRequest.DONE) {
                    if (xhr.status === 200) {
                        var res = JSON.parse(xhr.responseText)
                        root.request_token = res.code
                        ok()
                    } else {
                        error(xhr.getResponseHeader("X-Error-Code"))
                    }
                }
            }
        xhr.send(JSON.stringify(req))
    }

    function _accessToken(ok, error) {
        var req = {
            consumer_key: consumer_key,
            code: request_token
        }
        var xhr = new XMLHttpRequest()
        xhr.open("POST", oauth_authorize_url)
        xhr.setRequestHeader("Content-Type", "application/json; charset=UTF8")
        xhr.setRequestHeader("X-Accept", "application/json")
        xhr.onreadystatechange = function () {
                /*console.log("xhr.onreadystatechange")
                console.log("  xhr.readyState: " + xhr.readyState)
                console.log("  xhr.status: " + xhr.status)
                console.log("  xhr.responseType: " + xhr.responseType)
                console.log("  xhr.responseURL : " + xhr.responseURL )
                console.log("  xhr.statusText: " + xhr.statusText)
                console.log("  xhr.responseText: " + xhr.responseText)
                console.log("  X-Error-Code:" + xhr.getResponseHeader("X-Error-Code"))*/
                if(xhr.readyState === XMLHttpRequest.DONE) {
                    if (xhr.status === 200) {
                        var res = JSON.parse(xhr.responseText)
                        root.access_token = res.access_token
                        ok()
                    } else {
                        error(xhr.getResponseHeader("X-Error-Code"))
                    }
                }
            }
        xhr.send(JSON.stringify(req))
    }
}
