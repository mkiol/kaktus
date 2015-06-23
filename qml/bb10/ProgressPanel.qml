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
import bb.system 1.2
import "const.js" as Theme

Container {
    id: root

    property alias text: label.text
    property bool cancelable: true
    property bool open: true
    property alias progress: indicator.value
    property alias showProgress: indicator.visible
    signal cancelClicked

    layout: DockLayout {}
    
    preferredWidth: display.pixelSize.width
    minWidth: display.pixelSize.width
    maxWidth: display.pixelSize.width
    preferredHeight: utils.du(10)
    
    background: utils.plain()

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
    
    /*Container {
         id: indicator
         
         property double value
         
         horizontalAlignment: HorizontalAlignment.Left
         verticalAlignment: VerticalAlignment.Fill
         preferredWidth: value*display.pixelSize.width
         minWidth: value*display.pixelSize.width
         maxWidth: value*display.pixelSize.width
         background: utils.primary()
     }*/

    Container {
        layout: StackLayout {
            orientation: LayoutOrientation.LeftToRight
        }
        leftPadding: utils.du(2)
        rightPadding: utils.du(2)
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

        preferredWidth: utils.du(10)
        preferredHeight: utils.du(10)

        ImageView {
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
            //imageSource: "asset:///cancel.png"
            //filterColor: Color.create(Theme.hyperRedColor)
            imageSource: "asset:///cancel-red.png"
            preferredWidth: utils.du(8)
            preferredHeight: utils.du(8)
        }
    }

    ProgressIndicator {
        id: indicator
        horizontalAlignment: HorizontalAlignment.Left
        verticalAlignment: VerticalAlignment.Bottom
        preferredWidth: display.pixelSize.width
        minWidth: display.pixelSize.width
        maxWidth: display.pixelSize.width
    }

}