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
    property bool imageBackgroundVisible: true
    property int unreadCount
    property bool fresh
    property bool last: false
    property bool defaultIcon: false
    
    property int colorSize
    property int imageSize: Qt.utils.du(7)
    
    bottomPadding: last ? Qt.utils.du(15) : 0

    //background: pressed ? ui.palette.plainBase : ui.palette.background
    background: pressed ? Qt.utils.plainBase() : Qt.utils.background()
    
    onTouch: {
        if (!last)
            pressed = event.isDown()||event.isMove() ? true : false;
    }
    
    function setIconBgColor() {
        var r = root.text.length>0 ? (Math.abs(root.text.charCodeAt(0)-65)/57)%1 : 1;
        var g = root.text.length>1 ? (Math.abs(root.text.charCodeAt(1)-65)/57)%1 : 1;
        var b = root.text.length>2 ? (Math.abs(root.text.charCodeAt(2)-65)/57)%1 : 1;
        root.colorSize = (r+g+b);
        imagePlaceholder.background = Color.create(r, g, b)
        imagePlaceholderLabel.textStyle.color = (r+g+b)>1.5 ? Qt.utils.background() : Qt.utils.text();
    }
    
    layout: DockLayout {}

    Container {
        visible: !last
        verticalAlignment: VerticalAlignment.Top
        background: Qt.utils.plainBase()
        minHeight: 2
        maxHeight: minHeight
        minWidth: Qt.display.pixelSize.width
        maxWidth: minWidth
    }
    
    // Fresh dash
    /*ImageView {
        horizontalAlignment: HorizontalAlignment.Left
        visible: fresh
        //imageSource: "asset:///fresh_dash.png"
        //filterColor: ui.palette.primary
        imageSource: "asset:///fresh_dash-blue.png"
    }*/
    
    Container {
        id: body
        visible: !last
        verticalAlignment: VerticalAlignment.Top
        leftPadding: Qt.utils.du(2)
        rightPadding: leftPadding
        topPadding: leftPadding
        bottomPadding: leftPadding

        Container {
            layout: DockLayout {}
            
            preferredWidth: Qt.display.pixelSize.width

            Container {
                rightPadding: Qt.utils.du(5)
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                
                horizontalAlignment: HorizontalAlignment.Left

                Container {
                    layout: DockLayout {}
                    verticalAlignment: VerticalAlignment.Center
                    Container {
                        id: imagePlaceholder
                        layout: DockLayout {}
                        horizontalAlignment: HorizontalAlignment.Left
                        verticalAlignment: VerticalAlignment.Top
                        visible: !image.isLoaded || root.defaultIcon
                        preferredWidth: root.imageSize
                        preferredHeight: root.imageSize
                        minWidth: root.imageSize
                        minHeight: root.imageSize
                        maxWidth: root.imageSize
                        maxHeight: root.imageSize
                        Label {
                            visible: !root.defaultIcon
                            horizontalAlignment: HorizontalAlignment.Center
                            verticalAlignment: VerticalAlignment.Center
                            id: imagePlaceholderLabel
                            text: root.text.substring(0,1).toUpperCase()
                            textStyle.base: SystemDefaults.TextStyles.BodyText
                        }
                    }
                    
                    Container {
                        horizontalAlignment: HorizontalAlignment.Left
                        verticalAlignment: VerticalAlignment.Top
                        visible: image.isLoaded
                        background: root.imageBackgroundVisible ? Color.White : Color.Transparent
                        WebImageView {
                            id: image
                            preferredWidth: root.imageSize
                            preferredHeight: root.imageSize
                            minWidth: root.imageSize
                            minHeight: root.imageSize
                        }
                    }
                }

                Container {
                    bottomPadding: Qt.utils.du(1)
                    leftPadding: Qt.utils.du(2)
                    verticalAlignment: VerticalAlignment.Center
                    Label {
                        id: label
                        textStyle.base: SystemDefaults.TextStyles.PrimaryText
                        textStyle.color: unreadCount > 0 ? Qt.utils.text() : Qt.utils.secondaryText()
                        multiline: true
                        autoSize {
                            maxLineCount: 3
                        }
                    }
                }
            }

            Container {
                horizontalAlignment: HorizontalAlignment.Right
                leftPadding: Qt.utils.du(1)
                rightPadding: leftPadding
                topPadding: leftPadding
                bottomPadding: leftPadding
                //background: Qt.utils.plainBase()

                attachedObjects: [
                    ImagePaintDefinition {
                        id: border
                        repeatPattern: RepeatPattern.XY
                        imageSource: Application.themeSupport.theme.colorTheme.style == VisualStyle.Bright ? "asset:///border-bright.amd" : "asset:///border.amd"
                    }
                ]
                background: border.imagePaint

                Label {
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                    textStyle.color: Qt.utils.textOnPlain()
                    text: root.unreadCount
                }
            }
        }
    }
}