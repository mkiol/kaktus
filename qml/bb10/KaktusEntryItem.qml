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
import org.labsquare 1.0

Container {
    id: root

    property alias title: titleLabel.text
    property string author
    property alias content: contentLabel.text
    property alias feedIconSource: feedImage.url
    property string feedTitle
    property string annotations
    property alias imageSource: image.url
    property bool pressed: false
    property alias starPressed: star.pressed
    property alias barPressed: bar.pressed
    property bool read
    property bool stared
    property bool fresh
    property bool broadcast
    property bool liked
    property int date
    property bool expanded: false
    property bool last: false
    
    property bool defaultFeedIcon: false
    property int colorSize

    property bool hidden: read && !stared
    property bool showIcon: Qt.settings.viewMode==1 || Qt.settings.viewMode==3 || Qt.settings.viewMode==4 || Qt.settings.viewMode==5 ? true : false
    
    signal starClicked
    
    background: pressed ? Qt.utils.plainBase() : Qt.utils.background()
    preferredWidth: Qt.display.pixelSize.width
    bottomPadding: last ? Qt.utils.du(15) : 0
    
    function setIconBgColor() {
        var r = root.feedTitle.length>0 ? (Math.abs(root.feedTitle.charCodeAt(0)-65)/57)%1 : 1;
        var g = root.feedTitle.length>1 ? (Math.abs(root.feedTitle.charCodeAt(1)-65)/57)%1 : 1;
        var b = root.feedTitle.length>2 ? (Math.abs(root.feedTitle.charCodeAt(2)-65)/57)%1 : 1;
        root.colorSize = r+g+b;
        feedImagePlaceholder.background = Color.create(r, g, b);
        feedImagePlaceholderLabel.textStyle.color = (r+g+b)>1.5 ? Qt.utils.background() : Qt.utils.text();
    }

    onTouch: {
        if (!last)
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

    // Body stack
    Container {
        id: body
        visible: !last
        verticalAlignment: VerticalAlignment.Top

        // Header (feed icon, title, star)
        Container {
            leftPadding: Qt.utils.du(2)
            
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }

            Container {
                topPadding: Qt.utils.du(2)

                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }

                Container {
                    layout: DockLayout {}
                    verticalAlignment: VerticalAlignment.Top
                    rightPadding: Qt.utils.du(2)
                    topPadding: Qt.utils.du(1)
                    visible: root.showIcon
                    Container {
                        id: feedImagePlaceholder
                        layout: DockLayout {}
                        horizontalAlignment: HorizontalAlignment.Left
                        verticalAlignment: VerticalAlignment.Top
                        visible: !feedImage.isLoaded || root.defaultFeedIcon
                        preferredWidth: Qt.utils.du(5)
                        preferredHeight: Qt.utils.du(5)
                        minWidth: Qt.utils.du(5)
                        minHeight: Qt.utils.du(5)
                        maxWidth: Qt.utils.du(5)
                        maxHeight: Qt.utils.du(5)
                        Label {
                            horizontalAlignment: HorizontalAlignment.Center
                            verticalAlignment: VerticalAlignment.Center
                            visible: !root.defaultFeedIcon
                            id: feedImagePlaceholderLabel
                            text: root.feedTitle.substring(0,1).toUpperCase()
                            textStyle.base: SystemDefaults.TextStyles.BodyText
                        }
                    }
                    
                    Container {
                        horizontalAlignment: HorizontalAlignment.Left
                        verticalAlignment: VerticalAlignment.Top
                        visible: feedImage.isLoaded
                        background: root.defaultFeedIcon ? Color.Transparent : Color.White
                        WebImageView {
                            id: feedImage
                            preferredWidth: Qt.utils.du(5)
                            preferredHeight: Qt.utils.du(5)
                            minWidth: Qt.utils.du(5)
                            minHeight: Qt.utils.du(5)
                        }
                    }
                }

                Container {
                    verticalAlignment: VerticalAlignment.Top
                    preferredWidth: Qt.display.pixelSize.width
                    Label {
                        id: titleLabel
                        textStyle.base: SystemDefaults.TextStyles.PrimaryText
                        textStyle.color: root.hidden ? Qt.utils.secondaryText() : Qt.utils.text()
                        multiline: true
                        autoSize.maxLineCount: 4
                    }
                }

            }

            PressableContainer {
                id: star

                topPadding: Qt.utils.du(2)
                bottomPadding: Qt.utils.du(1)
                leftPadding: Qt.utils.du(1)
                rightPadding: Qt.utils.du(1)
                
                onClicked: {
                    starClicked()
                }
                
                ImageView {
                    imageSource: root.stared ? "asset:///star-selected-blue.png" : 
                    Application.themeSupport.theme.colorTheme.style==VisualStyle.Bright ? "asset:///star-text.png" : "asset:///star.png"
                    preferredHeight: Qt.utils.du(7)
                    preferredWidth: Qt.utils.du(7)
                    minHeight: Qt.utils.du(7)
                    minWidth: Qt.utils.du(7)
                }
            }
        }

        // Image
        Container {
            id: imageContainer
            leftPadding: image.width<Qt.display.pixelSize.width-2*Qt.utils.du(2) ? Qt.utils.du(2) : 0
            topPadding: Qt.utils.du(2)
            bottomPadding: 0
            
            visible: image.isLoaded && (!root.hidden || expanded)
            
            WebImageView {
                id: image
                doSizeCheck: true
                scalingMethod: ScalingMethod.AspectFit
                preferredWidth: image.width>Qt.display.pixelSize.width ? Qt.display.pixelSize.width : image.width
                maxWidth: image.width>Qt.display.pixelSize.width ? Qt.display.pixelSize.width : image.width
            }
        }

        // Content
        Container {
            leftPadding: Qt.utils.du(2)
            rightPadding: Qt.utils.du(2)
            topPadding: Qt.utils.du(1)
            bottomPadding: 0
            
            visible: contentLabel.text != "" && (!root.hidden || expanded)
            
            Label {
                id: contentLabel
                textStyle.base: SystemDefaults.TextStyles.BodyText
                textStyle.fontWeight: FontWeight.W100
                textStyle.color: root.hidden ? Qt.utils.secondaryText() : Qt.utils.text()
                multiline: true
                autoSize.maxLineCount: root.expanded ? 100 : 2
            }
        }

        // Bar
        PressableContainer {
            id: bar
            
            leftPadding: Qt.utils.du(2)
            rightPadding: Qt.utils.du(1)
            topPadding: Qt.utils.du(1)
            bottomPadding: 0
            
            onClicked: {
                expanded = !expanded;
            }
            
            Container {
                visible: (!root.hidden || root.expanded) && (root.annotations != "" || root.broadcast)
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                
                /*attachedObjects: [
                    ImagePaintDefinition {
                        id: border
                        repeatPattern: RepeatPattern.XY
                        imageSource: "asset:///border.amd"
                    }
                ]
                background: border.imagePaint*/
                
                ImageView {
                    preferredHeight: Qt.utils.du(6)
                    preferredWidth: Qt.utils.du(6)
                    minHeight: Qt.utils.du(6)
                    minWidth: Qt.utils.du(6)
                    imageSource: root.broadcast ?
                    Application.themeSupport.theme.colorTheme.style == VisualStyle.Bright ? "asset:///share-text.png" : "asset:///share.png" :
                    Application.themeSupport.theme.colorTheme.style == VisualStyle.Bright ? "asset:///comment-text.png" : "asset:///comment.png"
                }

                Container {
                    layout: DockLayout {}
                    visible: root.annotations != ""

                    Container {
                        horizontalAlignment: HorizontalAlignment.Left
                        verticalAlignment: VerticalAlignment.Fill
                        minHeight: Qt.utils.du(6)
                        minWidth: 3
                        background: Qt.utils.primary()
                    }

                    Container {
                        leftPadding: Qt.utils.du(2)
                        Label {
                            horizontalAlignment: HorizontalAlignment.Left
                            textStyle.base: SystemDefaults.TextStyles.SmallText
                            textStyle.color: root.hidden ? Qt.utils.secondaryText() : Qt.utils.text()
                            multiline: true
                            text: root.annotations
                        }
                    }
                }

            }
            
            /*Container {
                visible: !root.hidden || root.expanded
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                
                ImageView {
                    visible: root.broadcast
                    preferredHeight: Qt.utils.du(6)
                    preferredWidth: Qt.utils.du(6)
                    minHeight: Qt.utils.du(6)
                    minWidth: Qt.utils.du(6)
                    imageSource: Application.themeSupport.theme.colorTheme.style == VisualStyle.Bright ? "asset:///share-text.png" : "asset:///share.png"
                }
                
                ImageView {
                    visible: root.liked
                    preferredHeight: Qt.utils.du(6)
                    preferredWidth: Qt.utils.du(6)
                    minHeight: Qt.utils.du(6)
                    minWidth: Qt.utils.du(6)
                    imageSource: Application.themeSupport.theme.colorTheme.style == VisualStyle.Bright ? "asset:///like-text.png" : "asset:///like.png"
                }
                
            }*/

            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }

                Label {
                    verticalAlignment: VerticalAlignment.Center
                    preferredWidth: Qt.display.pixelSize.width
                    textStyle.base: SystemDefaults.TextStyles.SubtitleText
                    textStyle.fontWeight: FontWeight.W100
                    textStyle.color: root.hidden ? Qt.utils.secondaryText() : Qt.utils.text()
                    text: root.author != "" ? Qt.utils.getHumanFriendlyTimeString(date) + " • " + root.author : Qt.utils.getHumanFriendlyTimeString(date)
                }

                ImageView {
                    verticalAlignment: VerticalAlignment.Center
                    preferredHeight: Qt.utils.du(7)
                    preferredWidth: Qt.utils.du(7)
                    minHeight: Qt.utils.du(7)
                    minWidth: Qt.utils.du(7)
                    imageSource: Application.themeSupport.theme.colorTheme.style == VisualStyle.Bright ? "asset:///expand-text.png" : "asset:///expand.png"
                }
            }
        }
    }
}