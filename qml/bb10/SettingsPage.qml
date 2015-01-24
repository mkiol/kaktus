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

Page {

    id: root

    titleBar: TitleBar {
        title: qsTr("Settings")
    }
    
    property bool dark: Application.themeSupport.theme.colorTheme.style==VisualStyle.Dark
    
    function disconnectSignals() {
    }

    
    attachedObjects: [
        SignOutDialog {
            id:signOutDialog
            onOk: {
                settings.signedIn = false;
            }
        }
    ]
    
    Container {

    ScrollView {
        Container {
            Header {
                title: qsTr("Netvibes account")
            }

            Container {
                leftPadding: ui.du(2)
                rightPadding: ui.du(2)
                topPadding: ui.du(2)
                bottomPadding: ui.du(2)

                TextLabel {
                    text: settings.signedIn ? qsTr("Signed in as") : qsTr("Not signed in")
                    value: settings.signedIn ? settings.getNetvibesUsername() : ""
                    buttonText: settings.signedIn ? qsTr("Sign out") : qsTr("Sign in")
                    onClicked: {
                        if (settings.signedIn) {
                            signOutDialog.show();
                        } else {
                            nav.push(signInDialog.createObject());
                        }
                    }
                }

                TextLabel {
                    text: settings.signedIn && utils.defaultDashboardName() !== "" ? qsTr("Dashboard in use") : qsTr("Dashboard not selected")
                    value: settings.signedIn && utils.defaultDashboardName() !== "" ? utils.defaultDashboardName() : ""
                    buttonText: settings.signedIn ? qsTr("Change") : ""
                    onClicked: {
                        utils.setDashboardModel();
                        nav.push(dashboardPage.createObject());
                    }
                }
            }

            Header {
                title: qsTr("Cache")
            }

            Container {
                leftPadding: ui.du(2)
                rightPadding: ui.du(2)
                topPadding: ui.du(2)
                bottomPadding: ui.du(2)

                TextLabel {
                    text: qsTr("Current cache size")
                    value: utils.getHumanFriendlySizeString(dm.cacheSize)
                    buttonText: dm.cacheSize > 0 ? qsTr("Delete cache") : ""
                    onClicked: {
                        fetcher.cancel();
                        dm.cancel();
                        dm.removeCache();
                    }
                }

                DropDown {
                    title: qsTr("Network mode")
                    options: [
                        Option {
                            selected: ! settings.offlineMode
                            imageSource: dark ? "asset:///network-online.png" : "asset:///network-onlined.png"
                            value: false
                            text: qsTr("Online")
                        },
                        Option {
                            selected: settings.offlineMode
                            imageSource: dark ? "asset:///network-offline.png" : "asset:///network-offlined.png"
                            value: true
                            text: qsTr("Offline")
                        }
                    ]
                    onSelectedOptionChanged: {
                        settings.offlineMode = selectedOption.value;
                    }

                }

                ToggleComponent {
                    text: qsTr("Cache articles")
                    description: qsTr("After sync the content of all items will be downloaded " + "and cached for access in the Offline mode.")
                    checked: settings.autoDownloadOnUpdate

                    onCheckedChanged: {
                        settings.autoDownloadOnUpdate = checked;
                    }
                }
            }

            Header {
                title: qsTr("UI")
            }

            Container {

                leftPadding: ui.du(2)
                rightPadding: ui.du(2)
                topPadding: ui.du(2)
                bottomPadding: ui.du(2)

                DropDown {
                    title: qsTr("Language")
                    options: [
                        Option {
                            selected: settings.locale === ""
                            value: ""
                            text: qsTr("Default")
                        },
                        Option {
                            selected: settings.locale === "cs"
                            value: "cs"
                            text: "Čeština"
                        },
                        Option {
                            selected: settings.locale === "en"
                            value: "en"
                            text: "English"
                        },
                        Option {
                            selected: settings.locale === "fa"
                            value: "fa"
                            text: "فارسی"
                        },
                        Option {
                            selected: settings.locale === "nl"
                            value: "nl"
                            text: "Nederlands"
                        },
                        Option {
                            selected: settings.locale === "pl"
                            value: "pl"
                            text: "Polski"
                        },
                        Option {
                            selected: settings.locale === "ru"
                            value: "ru"
                            text: "Русский"
                        },
                        Option {
                            selected: settings.locale === "tr"
                            value: "tr"
                            text: "Türkçe"
                        }
                    ]
                    onSelectedOptionChanged: {
                        if (settings.locale != selectedOption.value) {
                            settings.locale = selectedOption.value;
                            notification.show(qsTr("Changes will take effect after you restart Kaktus."));
                        }

                    }
                }

                ViewModeDropDown {}

                ToggleComponent {
                    text: qsTr("Show only unread articles")
                    checked: settings.showOnlyUnread

                    onCheckedChanged: {
                        settings.showOnlyUnread = checked;
                    }
                }

                ToggleComponent {
                    text: qsTr("Show images")
                    checked: settings.showTabIcons

                    onCheckedChanged: {
                        settings.showTabIcons = checked;
                    }
                }

                /*ToggleComponent {
                    text: qsTr("Power save mode")
                    description: qsTr("When the phone or app goes to the idle state, " + "all opened web pages will be closed to lower power consumption.")
                    checked: settings.powerSaveMode

                    onCheckedChanged: {
                        settings.powerSaveMode = checked;
                    }
                }*/

                /*DropDown {
                    title: qsTr("Orientation")
                    options: [
                        Option {
                            selected: settings.allowedOrientations == value
                            value: 0
                            text: qsTr("Dynamic")
                        },
                        Option {
                            selected: settings.allowedOrientations == value
                            value: 1
                            text: qsTr("Portrait")
                        },
                        Option {
                            selected: settings.allowedOrientations == value
                            value: 2
                            text: qsTr("Landscape")
                        }
                    ]
                    onSelectedOptionChanged: {
                        settings.allowedOrientations = selectedOption.value;
                    }
                }*/
                
                DropDown {
                    title: qsTr("Theme")
                    options: [
                        /*Option {
                            selected: settings.theme == value
                            value: 0
                            text: qsTr("Default")
                        },*/
                        Option {
                            selected: settings.theme == value
                            value: 1
                            text: qsTr("Bright")
                        },
                        Option {
                            selected: settings.theme == value
                            value: 2
                            text: qsTr("Dark")
                        }
                    ]
                    onSelectedOptionChanged: {
                        settings.theme = selectedOption.value;
                    }
                }

                DropDown {
                    title: qsTr("Offline viewer style")
                    options: [
                        Option {
                            selected: settings.offlineTheme == value
                            value: String("black")
                            text: qsTr("Black")
                        },
                        Option {
                            selected: settings.offlineTheme == value
                            value: String("white")
                            text: qsTr("White")
                        }
                    ]
                    onSelectedOptionChanged: {
                        settings.offlineTheme = selectedOption.value;
                    }
                }

                DropDown {
                    title: qsTr("Web viewer font size")
                    options: [
                        Option {
                            selected: settings.fontSize == value
                            value: 0
                            text: qsTr("-50%")
                        },
                        Option {
                            selected: settings.fontSize == value
                            value: 1
                            text: qsTr("Normal")
                        },
                        Option {
                            selected: settings.fontSize == value
                            value: 2
                            text: qsTr("+50%")
                        }
                    ]
                    onSelectedOptionChanged: {
                        settings.fontSize = selectedOption.value;
                    }
                }
            }

            /*Header {
                title: qsTr("Other")
            }

            Container {

                leftPadding: ui.du(2)
                rightPadding: ui.du(2)
                topPadding: ui.du(2)
                bottomPadding: ui.du(2)

                Button {
                    text: qsTr("Show User Guide")

                    onClicked: {
                        //guide.show();
                        notification.show("Not implemented yet :-(");
                    }
                }

            }*/

        }
    }
}
}
