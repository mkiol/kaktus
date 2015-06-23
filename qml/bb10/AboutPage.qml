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
//import "const.js" as Theme

Page {
    
    id: root
    
    titleBar: TitleBar {
        title: qsTr("About")
        appearance: TitleBarAppearance.Plain
    }
    
    function disconnectSignals() {
    }

    
    actions: [
        ActionItem {
            title: qsTr("Changelog")
            ActionBar.placement: ActionBarPlacement.OnBar
            imageSource: "asset:///changelog.png"
            onTriggered: {
                nav.push(changelogPage.createObject());
            }
        }
    ]

    ScrollView {

        Container {

            preferredWidth: display.pixelSize.width
            verticalAlignment: VerticalAlignment.Center
            
            Container {
                topPadding: utils.du(4)
            }

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

            Label {
                horizontalAlignment: HorizontalAlignment.Center
                textStyle.textAlign: TextAlign.Center
                textStyle.color: utils.text()
                text: qsTr("Version: %1").arg(VERSION)
            }

            PaddingLabel {
                horizontalAlignment: HorizontalAlignment.Center
                textStyle.textAlign: TextAlign.Center
                text: qsTr("Multi services feed reader, specially designed to work offline.");
            }

            PaddingLabel {
                horizontalAlignment: HorizontalAlignment.Center
                textFormat: TextFormat.Html
                text: "<a href='%1'>%2</a>".arg(PAGE).arg(PAGE)
            }
            
            Container {
                topPadding: utils.du(6)
            }
            
            Header {
                title: qsTr("Copyright statement")
            }
            
            PaddingLabel {
                textStyle.textAlign: TextAlign.Left
                textFormat: TextFormat.Html
                text: "Copyright &copy; 2014-2015 Michal Kosciesza"
            }

            PaddingLabel {
                textStyle.textAlign: TextAlign.Left
                textFormat: TextFormat.Html
                text: qsTr("This software is distributed under the terms of the " + "<a href='https://www.gnu.org/licenses/gpl-3.0.txt'>GNU General Public Licence version 3</a>")
            }

            PaddingLabel {
                textStyle.textAlign: TextAlign.Left
                textFormat: TextFormat.Html
                text: qsTr("Be aware that Kaktus is an UNOFFICIAL application. It means is distributed in the hope " + "that it will be useful, but WITHOUT ANY WARRANTY. Without even the implied warranty of " + "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. " + "See the GNU General Public License for more details.")
            }

            Container {
                topPadding: utils.du(2)
            }
            
            /*Header {
                title: qsTr("Third-party copyrights")
            }
            
            PaddingLabel {
                textStyle.textAlign: TextAlign.Left
                text: qsTr("Kaktus utilizes third party software. Such third party software is copyrighted by their owners as indicated below.")
            }
            
            PaddingLabel {
                textFormat: TextFormat.Html
                text: "Copyright &copy; 2013, Kläralvdalens Datakonsult AB, a KDAB Group company"
            }
            
            PaddingLabel {
                textFormat: TextFormat.Html
                text: "Copyright &copy; 2011, Andre Somers"
            }
        
            PaddingLabel {
                textFormat: TextFormat.Html
                text: "Copyright &copy; 2011-2013 Nikhil Marathe"
            }
            
            PaddingLabel {
                textFormat: TextFormat.Html
                text: "Copyright &copy; 2008 Flavio Castelli"
            }
            
            Container {
                topPadding: utils.du(5)
            }*/
        }
    }
}
