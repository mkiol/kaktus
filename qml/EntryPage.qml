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


Page {
    id: root

    property string title

    SilicaListView {
        id: listView
        anchors.fill: parent
        model: entryModel

        MainMenu{}
        //UpMenu {}

        header: PageHeader {
            title: root.title
        }

        delegate: EntryDelegate {
            id: delegate
            title: model.title
            content: model.content
            date: model.date
            read: model.read
            readlater: model.readlater
            index: model.index

            onClicked: {
                var onlineUrl = model.link;
                var offlineUrl = cache.getUrl(model.uid);
                pageStack.push(Qt.resolvedUrl("WebViewPage.qml"),
                               {"entryId": model.uid,
                                   "onlineUrl": onlineUrl,
                                   "offlineUrl": offlineUrl,
                                   "title": model.title
                               });
            }
        }

        ViewPlaceholder {
            enabled: listView.count == 0
            text: qsTr("No entries")
        }

        VerticalScrollDecorator {
            flickable: listView
        }

    }

}
