import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Layouts 1.3
import Qt.labs.controls 1.0
import Qt.labs.controls.material 1.0
import banana.components 1.0

ApplicationWindow {
    id: root

    visible: true
    title: qsTr("Banana Downloader")
    width: 800
    height: 600
    minimumWidth: 800
    minimumHeight: 600

    Material.accent: Material.Yellow
    Material.theme: Material.Dark

    ListModel {
        id: downloads
    }

    Rectangle {
        id: logo
        width: parent.width
        height: 100
        color: "transparent"
    }

    Item {
        id: uiWrapper
        anchors.top: logo.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10

        Row {
            id: buttons

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right

            height: 40
            spacing: 10

            Button {
                text: "New Download"
                onClicked: newDownloadPopup.open()
            }
        }

        ListView {
            id: downloadViewer

            anchors.top: buttons.bottom
            anchors.topMargin: 10
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            spacing: 10

            model: downloads

            delegate: Rectangle {
                width: parent.width
                height: 100
                radius: 10
                color: "#1F000000"
                border.width: 1
                border.color: "#3F000000"
                Column {
                    anchors.fill: parent
                    anchors.margins: 10

                    Label {
                        text: qsTr("Download URL\t: %1").arg(item.url)
                        width: parent.width - 50
                        font.pointSize: 13
                        elide: Text.ElideRight
                    }
                    Label {
                        width: parent.width - 50
                        text: qsTr("Download to File\t: %1").arg(item.file)
                        font.pointSize: 13
                        elide: Text.ElideRight
                    }
                    Label {
                        text: qsTr("%1\% has been downloaded").arg(item.progress * 100)
                        font.pointSize: 13
                    }
                }
                ProgressBar {
                    implicitWidth:  parent.width - 20
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.margins: 10

                    value: item.progress
                }
            }
        }
    }

    Popup {
        id: newDownloadPopup
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        width: 320
        height: 240
        modal: true
        focus: true
        closePolicy: Popup.OnEscape | Popup.OnPressOutside
        topPadding: 15
        leftPadding: 10
        rightPadding: 10

        Column {
            Label {
                font.pointSize: 16
                width: 300
                text: "<b>New Download</b>"
                color: "moccasin"
                horizontalAlignment: TextInput.AlignHCenter
            }
            Rectangle {
                width: 300
                height: 20
                color: "transparent"
            }

            Column {
                Label {
                    font.pointSize: 12
                    text: "Source path to download :"
                    color: "moccasin"
                }
                TextField {
                    id: url
                    text: "http://www.google.com"
                    font.pointSize: 12
                    implicitWidth: 300
                    placeholderText: qsTr("Enter source URL")
                }
            }

            Column {
                Label {
                    font.pointSize: 12
                    text: "File name:"
                    color: "moccasin"
                }
                TextField {
                    id: file
                    text: "D:/index.html"
                    font.pointSize: 12
                    implicitWidth: 300
                    placeholderText: qsTr("Enter file name")
                }
            }

            Row {
                width: 300
                spacing: 10
                layoutDirection: Qt.RightToLeft
                Button {
                    enabled: url.length && file.length
                    text: "Start"
                    onClicked: {
                        downloadFile();
                        newDownloadPopup.close();
                    }
                }
                Button {
                    text: "Cancel"
                    onClicked: newDownloadPopup.close();
                }
            }
        }
    }

    Component {
        id: downloadItem
        DownloadProgress {
            property string log: ""
            Component.onCompleted: {
                appendLog.connect(function(newLog) {
                    log = log + "\n" + newLog;
                })
            }
            Component.onDestruction: {
                safeRemove();
            }
        }
    }

    function downloadFile()
    {
        var obj = {};
        obj["item"] = downloadItem.createObject(root,
            {"url": url.text, "file": file.text});
        downloads.append(obj);
        obj["item"].startDownload();
    }

    function addAnElement(newDownload) {
        downloads.append(newDownload);
    }

    Component.onCompleted: {
    }
}
