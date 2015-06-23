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
import "const.js" as Theme

Page {
    id: root

    property bool menuEnabled: false
    property int code

    function disconnectSignals() {
    }

    titleBar: TitleBar {
        title: qsTr("Account")
    }

    ScrollView {

        Container {
            topPadding: utils.du(2)
            bottomPadding: utils.du(2)

            /*PaddingLabel {
                text: qsTr("Only connecting with Netvibes and Twitter credentials are supported right now.")
                textStyle.color: utils.primary()
            }*/

            Header {
                title: qsTr("Old Reader")
            }

            Container {
                leftPadding: utils.du(2)
                rightPadding: utils.du(2)
                topPadding: utils.du(2)
                bottomPadding: utils.du(2)
                
                Container {
                    preferredWidth: display.pixelSize.width
                    
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }

                    Container {
                        topPadding: utils.du(1)
                        ImageView {
                            imageSource: "asset:///oldreader.png"
                            verticalAlignment: VerticalAlignment.Top
                            preferredWidth: 46
                            minWidth: 46
                        }
                    }

                    Label {
                        text: qsTr("Use your credentials to configure the account. Enter username and password below.")
                        multiline: true
                        verticalAlignment: VerticalAlignment.Top
                    }
                }

                Label {
                    text: qsTr("Username (your e-mail)")
                }

                TextField {
                    id: user
                    hintText: qsTr("Enter username here!")
                    inputMode: TextFieldInputMode.EmailAddress

                    validator: Validator {
                        mode: ValidationMode.FocusLost
                        errorMessage: "Your e-mail address is your username."
                        onValidate: {
                            if (validateEmail(user.text))
                                state = ValidationState.Valid;
                            else
                                state = ValidationState.Invalid;
                        }
                    }

                    onCreationCompleted: {
                        text = settings.getUsername();
                    }

                    input.submitKey: SubmitKey.Connect
                    input.onSubmitted: {
                        password.requestFocus();
                    }
                }
                
                Label {
                    text: qsTr("Password")
                }

                TextField {
                    id: password
                    hintText: qsTr("Enter password here!")
                    inputMode: TextFieldInputMode.Password
                    validator: Validator {
                        mode: ValidationMode.FocusLost
                        errorMessage: "Your password should not be empty."
                        onValidate: {
                            if (password.text.length > 0)
                                state = ValidationState.Valid;
                            else
                                state = ValidationState.Invalid;
                        }
                    }

                    input.submitKey: SubmitKey.Connect
                    input.onSubmitted: {
                        accept();
                    }
                }

            }
            
            Container {
                minWidth: utils.du(2)
            }

        }
    }

    actions: [
        ActionItem {
            id: action1
            title: qsTr("Sign in")
            imageSource: "asset:///save_folder.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                accept();
            }
        }
    ]

    function validateEmail(email) {
        var re = /^(([^<>()[\]\\.,;:\s@\"]+(\.[^<>()[\]\\.,;:\s@\"]+)*)|(\".+\"))@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\])|(([a-zA-Z\-0-9]+\.)+[a-zA-Z]{2,}))$/;
        return re.test(email);
    } 

    function accept() {
        
        if (user.text == "")
            return;
        
        settings.setUsername(user.text);
        
        if (password.text == "")
            return;

        settings.setPassword(password.text);
        
        var doInit = settings.signinType!=0;
        settings.signinType = 10;

        if (!dm.busy)
            dm.cancel();
        if (doInit)
            fetcher.init();
        else
            fetcher.update();

        nav.pop();
    }
}
