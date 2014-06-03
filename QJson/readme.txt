Project to manage Json focused on QML.

---------

Project summary
Categories: 	MeeGo Harmattan, Qt, Qt Quick
Created: 	April 5th, 2012
Visibility: 	Private
Followers:	4
Downloads:

---------

Wiki:

Introduction

Starting from ?QJson project published in 2009, I have modified and updated the code with new features and easier accessible from QML. In addition to the existing methods, has been added those that takes as input a URL. This is very useful to save code development especially in QML
Get the code

    Download source code from ?here
    Include QJson directory into your project 

.pro

include(./qjson/json.pri)

main.cpp

#include <QtGui/QApplication>
#include "qmlapplicationviewer.h"

#include <QtDeclarative>
#include "qjson/qjson.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));

    qmlRegisterType<QJson>("QJson", 1, 0, "QJson");

    QmlApplicationViewer viewer;
    viewer.setMainQmlFile(QLatin1String("qml/QJsonSample/main.qml"));
    viewer.showExpanded();

    return app->exec();
}

How to fill a ListView? using Json

The json sample is also available ?here.

{
    "encoding": "UTF-8",
    "plug-ins": [
        "python",
        "c++",
        "ruby",
        "java"
    ],
    "indent": {
        "length": 3,
        "use_space": true
    },
    "people": [
        {
            "name": "Sebastiano",
            "surname": "Galazzo",
            "email": "sebastiano.galazzo@gmail.com"
        },
        {
            "name": "John",
            "surname": "Doe",
            "email": "john.doe@gmail.com"
        }
    ]
}

QML

import QtQuick 1.1
import Qt 4.7
import com.nokia.symbian 1.1
import QJson 1.0

Page {
    id: mainPage

    Column {
        width:parent.width
        height: parent.height
        spacing: 20

        Text {
            id:encoding
            color: "white"
        }

        Rectangle {
            width: parent.width
            height: 100

            ListModel {
                id:plugins
            }
            ListView {
                width: 360
                height: 555
                anchors.fill: parent
                model: plugins
                clip:true
                delegate: Text { text: plugin }
            }
        }

        Rectangle {
            width: parent.width
            height: 100

            ListModel {
                id:indent
            }

            ListView {
                width: 360
                height: 555
                anchors.fill: parent
                model: indent
                clip:true
                delegate: Column {
                            spacing: 10
                            Text { text: length }
                            Text { text: use_space }
                        }
            }
        }

        Rectangle {
            width: parent.width
            height: 100

            ListModel {
                id:people
            }

            ListView {
                width: 360
                height: 555
                anchors.fill: parent
                model: people
                spacing: 10
                clip:true
                delegate: Column {
                            Row {
                                spacing: 10
                                Text { text: name }
                                Text { text: surname }
                            }
                            Text {
                                text: email
                                MouseArea{
                                    anchors.fill: parent
                                    onClicked: {
                                        Qt.openUrlExternally("mailto:"+email)
                                    }
                                }
                            }
                        }
            }
        }
    }

    QJson {
        id: json
        onError: {
            console.log("line:"+line)
            console.log("message:"+message)
        }
    }

    Component.onCompleted: {

        var data = json.parse("http://www.witinside.net/json/sample.json")

        encoding.text = "Encoding: "+data['encoding'];

        var json_plugins = data["plug-ins"];
        for(var elem in json_plugins){
            plugins.append({"plugin": json_plugins[elem]})
        }

        var json_indent = data["indent"];
        indent.append({
                           "length": "Length: "+ json_indent["length"]
                          ,"use_space": "Use space: "+ json_indent["use_space"]
                    })

        var json_people = data["people"];
        for(var index in json_people){
            people.append({
                              "name"   : json_people[index]["name"]
                             ,"surname": json_people[index]["surname"]
                             ,"email"  : json_people[index]["email"]
                       })
        }
    }
}