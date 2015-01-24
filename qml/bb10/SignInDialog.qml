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
    
    property bool menuEnabled: false
    property int code

    titleBar: TitleBar {
        title: "Netvibes Account"
    }

    Container {
        leftPadding: ui.du(2)
        rightPadding: ui.du(2)
        topPadding: ui.du(2)
        bottomPadding: ui.du(2)
        
        Label {
            text: qsTr("Use your credentials to configure the account. "+
            "Enter username (your e-mail) and password below.");
            multiline: true
        }
        
        Label {
            text: qsTr("Only connecting with Netvibes credentials are supported right now. "+ 
            "Twitter, Facebook and Google+ sign in option will be added in next release.");
            textStyle.color: Application.themeSupport.theme.colorTheme.style==VisualStyle.Bright ?
            Color.create(Theme.secondaryBrightColor) : Color.create(Theme.secondaryDarkColor)
            multiline: true
        }
        
        Container {
            topPadding: ui.du(1)
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
                text = settings.getNetvibesUsername();
            }
            
            input.submitKey: SubmitKey.Connect
            input.onSubmitted: {
                password.requestFocus();
            }
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

    actions: [
        ActionItem {
            id: action1
            enabled: user.text!=""
            title: qsTr("Sign in")
            imageSource: "asset:///add.png"
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
        
        settings.setNetvibesUsername(user.text);
        
        if (password.text == "")
            return;

        settings.setNetvibesPassword(password.text);

        if (code == 0) {
            fetcher.checkCredentials();
        } else {
            if (! dm.busy)
                dm.cancel();
            fetcher.update();
        }

        nav.pop();
    }
}
