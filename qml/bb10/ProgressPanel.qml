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
import "const.js" as Theme

Container {
    id: root

    property alias text: label.text
    property bool cancelable: true
    property bool open: true
    property alias progress: indicator.value
    signal cancelClicked

    layout: DockLayout {}
    minWidth: display.pixelSize.width
    preferredHeight: ui.du(9)
    
    background: ui.palette.plain

    enabled: open
    visible: open

    function show(text) {
        root.progress = -1.0;
        root.text = text;
        root.open = true;
    }

    function hide() {
        root.open = false;
        root.progress = -1.0;
    }

    Container {
        layout: StackLayout {
            orientation: LayoutOrientation.LeftToRight
        }
        leftPadding: ui.du(2)
        rightPadding: ui.du(2)
        verticalAlignment: VerticalAlignment.Center
        horizontalAlignment: HorizontalAlignment.Left

        ActivityIndicator {
            running: root.open
            verticalAlignment: VerticalAlignment.Center
        }

        Label {
            id: label
            verticalAlignment: VerticalAlignment.Center
            multiline: true
            visible: text != ""
        }
    }

    PressableContainer {
        layout: DockLayout {
        }
        visible: root.cancelable
        horizontalAlignment: HorizontalAlignment.Right
        verticalAlignment: VerticalAlignment.Center

        onClicked: {
            root.cancelClicked();
        }

        preferredWidth: ui.du(12)
        preferredHeight: ui.du(9)

        ImageView {
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
            imageSource: "asset:///cancel.png"
            filterColor: Color.create(Theme.hyperRedColor)
            preferredWidth: ui.du(7)
            preferredHeight: ui.du(7)
        }
    }

    ProgressIndicator {
        id: indicator
        horizontalAlignment: HorizontalAlignment.Left
        verticalAlignment: VerticalAlignment.Bottom
    }

}