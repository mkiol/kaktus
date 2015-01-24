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

Page {
    
    id: root
    
    titleBar: TitleBar {
        title: qsTr("About")
        appearance: TitleBarAppearance.Plain
    }
    
    function disconnectSignals() {
    }

    
    /*actions: [
        ActionItem {
            title: qsTr("Changelog")
            ActionBar.placement: ActionBarPlacement.OnBar
            imageSource: "asset:///changelog.png"
            onTriggered: {
                nav.push(changelogPage.createObject());
            }
        }
    ]*/

    ScrollView {

        Container {
            leftPadding: ui.du(2)
            rightPadding: ui.du(2)
            topPadding: ui.du(4)
            bottomPadding: ui.du(2)

            preferredWidth: display.pixelSize.width
            verticalAlignment: VerticalAlignment.Center

            ImageView {
                horizontalAlignment: HorizontalAlignment.Center
                imageSource: "asset:///icon.png"
            }

            Label {
                horizontalAlignment: HorizontalAlignment.Center
                textStyle.textAlign: TextAlign.Center
                textStyle.base: SystemDefaults.TextStyles.BigText
                text: APP_NAME
            }

            /*Container {
             * horizontalAlignment: HorizontalAlignment.Center
             * background: ui.palette.primary
             * 
             * leftPadding: ui.du(1)
             * rightPadding: ui.du(1)
             * topPadding: ui.du(1)
             * bottomPadding: ui.du(1)
             * 
             * Label {
             * horizontalAlignment: HorizontalAlignment.Center
             * textStyle.textAlign: TextAlign.Center
             * textStyle.color: ui.palette.textOnPrimary
             * text: qsTr("Version: %1").arg(VERSION);
             * }
             }*/

            Label {
                horizontalAlignment: HorizontalAlignment.Center
                textStyle.textAlign: TextAlign.Center
                textStyle.color: ui.palette.text
                text: qsTr("Version: %1").arg(VERSION)
            }

            Label {
                horizontalAlignment: HorizontalAlignment.Center
                textStyle.textAlign: TextAlign.Center
                text: qsTr("An unofficial Netvibes feed reader, specially designed to work offline.")
                multiline: true
            }

            Label {
                horizontalAlignment: HorizontalAlignment.Center
                textFormat: TextFormat.Html
                text: "<a href='%1'>%2</a>".arg(PAGE).arg(PAGE)
            }

            Label {
                horizontalAlignment: HorizontalAlignment.Center
                textStyle.textAlign: TextAlign.Center
                textFormat: TextFormat.Html
                text: "Copyright &copy; 2014 Michał Kościesza"
            }

        }
    }
}
