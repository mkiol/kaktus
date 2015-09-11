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

Page {
    id: root
    
    titleBar: TitleBar {
        title: qsTr("Changelog")
    }
    
    Container {
        ScrollView {
            Container {
                Header {
                    title: qsTr("Version %1").arg("2.1")
                }
                
                Container {
                    leftPadding: utils.du(2)
                    rightPadding: utils.du(2)
                    topPadding: utils.du(2)
                    bottomPadding: utils.du(2)
                    
                    LogItem {
                        title: 'Sync read articles'
                        description: "Previously, unread as well as read articles were synced. With this new option comes possiblity to disable syncing read articles. It will speed up synchronization, but read articles will not be accessible form Kaktus.";
                    }

                    LogItem {
                        title: 'Sort order for list of articles'
                        description: 'New settings option enabling specific sort order for list of articles. Possible values: Recent first, Oldest first.'
                    }
                    
                    LogItem {
                        title: 'Mark above as read'
                        description: 'Context menu for list of articles has new option for marking all above articles as read.'
                    }
                    
                    LogItem {
                        title: 'Old Reader: Like & Liked articles tab'
                        description: 'New context option to Like/Unlike article. So called "Slow" tab is now replaced by Liked articles tab.'
                    }
                    
                    LogItem {
                        title: 'Old Reader: Enable social features'
                        description: 'New option to enable/disable Old Reader\'s social features. If enabled, following features will be visible: Following folder, Sharing article with followers, Like/Unlike option, Liked articles tab.'
                    }
                }
                Header {
                    title: qsTr("Version %1").arg("2.0")
                }
                
                Container {
                    leftPadding: utils.du(2)
                    rightPadding: utils.du(2)
                    topPadding: utils.du(2)
                    bottomPadding: utils.du(2)
                    
                    LogItem {
                        title: 'Old Reader support'
                        description: 'Old Reader is supported as new feed aggreagator.'
                    }

                    LogItem {
                        title: 'Many small improvements and bug fixes'
                        description: 'Many improvements, like performance optimization and UI polishing were made.'
                    }
                }
                
                Header {
                    title: qsTr("Version %1").arg("1.3")
                }
                
                Container {
                    leftPadding: utils.du(2)
                    rightPadding: utils.du(2)
                    topPadding: utils.du(2)
                    bottomPadding: utils.du(2)
                    
                    LogItem {
                        title: 'Read mode'
                        description: 'When Read mode is enabled, web pages will be reformatted into an easy to read version. '+
                        'All of a website\'s native styles will be striped so you can focus on what you\'re reading. '+
                        'You can switch to Read mode using action on web viewer\'s toolbar.'
                    }
                    
                    LogItem {
                        title: 'Copy & Share URL actions'
                        description: 'New actions were added to web viewer\'s toolbar. Allows you to copy page\'s URL to clipboard or share.'
                    }
                    
                    LogItem {
                        title: 'Sign in with Twitter or Facebook'
                        description: 'In addition to Netvibes credentials, sign in can be done also with Twitter or Facebook account.'
                    }
                                
                    LogItem {
                        title: 'Auto-hide bars & many other UI improvements'
                        description: 'Many UI improvements were made. E.g. title bar and action bar are auto-hidden.'
                    }
                }
                
                Header {
                    title: qsTr("Version %1").arg("1.2.3")
                }

                Container {
                    leftPadding: utils.du(2)
                    rightPadding: utils.du(2)
                    topPadding: utils.du(2)
                    bottomPadding: utils.du(2)

                    LogItem {
                        title: 'Initial version for BlackBerry'
                        description: 'This is first version of Kaktus for BlackBerry 10.'
                    }
                }
            }
        }

    }
}
