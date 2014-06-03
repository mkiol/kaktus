import QtQuick 1.1
import com.nokia.meego 1.0

Page {
    id: root

    tools: MainToolbar {}

    Column {

        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom; bottomMargin: UiConstants.DefaultMargin
        }

        spacing: UiConstants.ButtonSpacing

        Button{
            text: qsTr("Show Tabs")

            onClicked: {
                pageStack.replace(Qt.resolvedUrl("TabPage.qml"))
            }
        }

        Button{
            text: qsTr("Sync")

            onClicked: {
                notification.show("Sync started!");
                fetcher.update();
            }
        }


    }

}
