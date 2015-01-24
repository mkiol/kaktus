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
import org.labsquare 1.0
import "const.js" as Theme

Container {
    id: root

    property alias text: label.text
    property alias imageSource: image.url
    property bool pressed
    property int unreadCount
    property bool fresh

    background: pressed ? ui.palette.plainBase : ui.palette.background

    onTouch: {
        pressed = event.isDown()||event.isMove() ? true : false;
    }
    
    layout: DockLayout {}

    Container {
        verticalAlignment: VerticalAlignment.Top
        background: ui.palette.plainBase
        minHeight: 2
        maxHeight: minHeight
        minWidth: Qt.display.pixelSize.width
        maxWidth: minWidth
    }
    
    // Fresh dash
    ImageView {
        horizontalAlignment: HorizontalAlignment.Left
        visible: fresh
        imageSource: "asset:///fresh_dash.png"
        filterColor: ui.palette.primary
    }

    Container {
        id: body
        verticalAlignment: VerticalAlignment.Top
        leftPadding: ui.du(2)
        rightPadding: ui.du(2)
        topPadding: ui.du(2)
        bottomPadding: ui.du(2)

        //verticalAlignment: VerticalAlignment.Center

        Container {
            layout: DockLayout {
            }
            
            preferredWidth: Qt.display.pixelSize.width

            Container {
                rightPadding: ui.du(5)
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                
                horizontalAlignment: HorizontalAlignment.Left

                WebImageView {
                    id: image
                    visible: isLoaded
                    //verticalAlignment: VerticalAlignment.Center
                    preferredWidth: ui.du(5)
                    preferredHeight: preferredWidth
                    minWidth: ui.du(5)
                    minHeight: minWidth
                }

                Label {
                    id: label
                    textStyle.base: SystemDefaults.TextStyles.PrimaryText
                    textStyle.color: unreadCount>0
                        ? ui.palette.text :
                        Application.themeSupport.theme.colorTheme.style==VisualStyle.Bright ?
                        Color.create(Theme.secondaryBrightColor) : Color.create(Theme.secondaryDarkColor)
                    multiline: true
                    autoSize {
                        maxLineCount: 3
                    }
                }
            }

            Container {
                horizontalAlignment: HorizontalAlignment.Right
                leftPadding: ui.du(1)
                rightPadding: ui.du(1)
                topPadding: ui.du(1)
                bottomPadding: ui.du(1)
                background: ui.palette.plainBase
                Label {
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                    textStyle.color: ui.palette.textOnPlain
                    text: root.unreadCount
                }
            }
        }
    }
}