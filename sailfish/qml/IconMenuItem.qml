/**
 *
 * gPodder QML UI Reference Implementation
 * Copyright (c) 2013, 2014, Thomas Perl <m@thp.io>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */

import QtQuick 2.0
import Sailfish.Silica 1.0

Item {
    id: root
    property alias icon: iconButton.icon
    property alias enabled: iconButton.enabled
    property alias text: lbl.text
    property bool busy: false

    width: iconButton.width
    height: iconButton.height

    signal clicked()

    Label {
        id: lbl
        opacity: iconButton.down

        Behavior on opacity {
            FadeAnimation {}
        }

        anchors {
            verticalCenter: parent.top
            horizontalCenter: parent.horizontalCenter
        }

        color: Theme.highlightColor
        font.pixelSize: Theme.fontSizeTiny
        wrapMode: Text.Wrap
        horizontalAlignment: Text.AlignHCenter
    }

    Image {
        anchors.fill: parent
        opacity: root.busy ? 1.0 : 0.0
        visible: opacity > 0.0
        Behavior on opacity { FadeAnimation {} }
        source: "image://theme/graphic-busyindicator-medium"
        RotationAnimation on rotation {
            loops: Animation.Infinite
            from: 0
            to: 360
            duration: 1200
            running: root.busy && Qt.application.active
        }
    }

    IconButton {
        id: iconButton
        onClicked: root.clicked();
    }
}
