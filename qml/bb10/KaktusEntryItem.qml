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

    property alias title: titleLabel.text
    property string author
    property alias content: contentLabel.text
    property alias feedIconSource: feedImage.url
    property alias imageSource: image.url
    property bool pressed: false
    property alias starPressed: star.pressed
    property alias barPressed: bar.pressed
    property bool read
    property bool stared
    property bool fresh
    property int date
    property bool expanded: false

    property bool hidden: read && !stared
    
    signal starClicked
    
    background: pressed ? ui.palette.plainBase : ui.palette.background
    preferredWidth: Qt.display.pixelSize.width

    onTouch: {
        pressed = !barPressed && !starPressed && (event.isDown()||event.isMove()) ? true : false;
    }
    
    onHiddenChanged: {
        if (hidden) {
            expanded = false;
        }
    }
    
    layout: DockLayout {}

    // Top line
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

    // Body stack
    Container {
        id: body

        verticalAlignment: VerticalAlignment.Top
        //background: Color.Red
        
        // Header (feed icon, title, star)
        Container {
            leftPadding: ui.du(2)
            rightPadding: ui.du(0)
            topPadding: ui.du(0)
            bottomPadding: ui.du(0)
            
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }

            //background: Color.Blue

            Container {
                topPadding: ui.du(2)
                
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                WebImageView {
                    id: feedImage
                    visible: isLoaded
                    preferredWidth: ui.du(5)
                    preferredHeight: preferredWidth
                    minWidth: ui.du(5)
                    minHeight: minWidth
                }

                Label {
                    id: titleLabel
                    textStyle.base: SystemDefaults.TextStyles.PrimaryText
                    textStyle.color: root.hidden ? 
                    Application.themeSupport.theme.colorTheme.style==VisualStyle.Bright ?
                    Color.create(Theme.secondaryBrightColor) : Color.create(Theme.secondaryDarkColor) :
                    ui.palette.text
                    preferredWidth: Qt.display.pixelSize.width
                    multiline: true
                    autoSize.maxLineCount: 4
                }
            }

            PressableContainer {
                id: star

                topPadding: ui.du(1)
                bottomPadding: ui.du(1)
                leftPadding: ui.du(1)
                rightPadding: ui.du(1)
                
                onClicked: {
                    starClicked()
                }
                
                ImageView {
                    imageSource: root.stared ? "asset:///star-selected.png" : "asset:///star.png"
                    filterColor: root.stared ? ui.palette.primary : ui.palette.text
                    preferredHeight: ui.du(7)
                    preferredWidth: ui.du(7)
                    minHeight: preferredHeight
                    minWidth: preferredWidth
                }
            }
        }

        // Image
        Container {
            leftPadding: ui.du(2)
            rightPadding: ui.du(2)
            topPadding: ui.du(1)
            bottomPadding: ui.du(0)
            
            visible: image.isLoaded && (!root.hidden || expanded)
            
            WebImageView {
                id: image
                scalingMethod: ScalingMethod.AspectFit
            }
        }

        // Content
        Container {
            leftPadding: ui.du(2)
            rightPadding: ui.du(2)
            topPadding: ui.du(1)
            bottomPadding: ui.du(0)
            
            visible: contentLabel.text != "" && (!root.hidden || expanded)
            
            Label {
                id: contentLabel
                textStyle.base: SystemDefaults.TextStyles.BodyText
                textStyle.fontWeight: FontWeight.W100
                textStyle.color: hidden ?
                Application.themeSupport.theme.colorTheme.style==VisualStyle.Bright ?
                Color.create(Theme.secondaryBrightColor) : Color.create(Theme.secondaryDarkColor) :
                ui.palette.text
                multiline: true
                autoSize.maxLineCount: root.expanded ? 100 : 2
            }
        }

        // Bar
        PressableContainer {
            id: bar
            
            leftPadding: ui.du(2)
            rightPadding: ui.du(1)
            topPadding: ui.du(1)
            bottomPadding: ui.du(0)
            
            onClicked: {
                expanded = !expanded;
            }
            
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            
            Label {
                verticalAlignment: VerticalAlignment.Center
                preferredWidth: Qt.display.pixelSize.width
                textStyle.base: SystemDefaults.TextStyles.SubtitleText
                textStyle.fontWeight: FontWeight.W100
                textStyle.color: hidden ?
                Application.themeSupport.theme.colorTheme.style==VisualStyle.Bright ?
                Color.create(Theme.secondaryBrightColor) : Color.create(Theme.secondaryDarkColor) :
                ui.palette.text
                text: root.author!=""
                ? Qt.utils.getHumanFriendlyTimeString(date)+" • "+root.author
                : Qt.utils.getHumanFriendlyTimeString(date)
            }
            
            ImageView {
                verticalAlignment: VerticalAlignment.Center
                preferredHeight: ui.du(7)
                preferredWidth: ui.du(7)
                minHeight: preferredHeight
                minWidth: preferredWidth
                
                imageSource: "asset:///expand.png"
                filterColor: ui.palette.text
            }
        }
    }
}