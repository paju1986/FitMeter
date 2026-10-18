import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: window

    width: 520
    height: 600

    visible: true
    title: "H9 Heart Rate"
    color: "#101318"

            property int stopwatchTime: 0

            function updateTimerLabel() {
                let seconds = stopwatchTime % 60
                let minutes = Math.floor((stopwatchTime / 60) % 60)
                let hours = Math.floor(stopwatchTime / 3600)
                timerLabel.text =
                    (hours < 10 ? "0" + hours : hours) + ":" +
                    (minutes < 10 ? "0" + minutes : minutes) + ":" +
                    (seconds < 10 ? "0" + seconds : seconds)
            }

    Flickable {
        anchors.fill: parent
        anchors.margins: 20
        contentHeight: mainLayout.implicitHeight
        clip: true

        ColumnLayout {
            id: mainLayout
            width: parent.width
            spacing: 15

            Label {
                Layout.alignment: Qt.AlignHCenter
                text: "POLAR H9"
            color: "white"
                font.pixelSize: 24
                font.bold: true
            }

            Label {
                Layout.alignment: Qt.AlignHCenter
                text: heartRateManager.deviceName !== ""
                      ? heartRateManager.deviceName
                      : "No sensor"
                color: "#9aa4b2"
                font.pixelSize: 14
            }

            Rectangle {
            Layout.alignment: Qt.AlignHCenter
                width: 200
                height: 200
                radius: width / 2
                color: "#191e26"
                border.width: 5
                border.color: heartRateManager.connected ? "#e53935" : "#343b47"

                Column {
                    anchors.centerIn: parent
                    spacing: 2
                    Label {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: heartRateManager.heartRate > 0 ? heartRateManager.heartRate : "--"
                        color: "white"
                        font.pixelSize: 50
                        font.bold: true
                    }
                    Label {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "BPM"
                        color: "#e53935"
                        font.pixelSize: 16
                        font.bold: true
                    }
                }

                MouseArea {
                    anchors.fill: parent
                        onClicked: {
                        if (heartRateManager.connected) {
                            heartRateManager.disconnectDevice()
                            } else {
                            heartRateManager.startScan()
                        }
                    }
                }
            }
            Label {
                Layout.alignment: Qt.AlignHCenter
                text: heartRateManager.status
                color: "#9aa4b2"
                font.pixelSize: 15
        }

            Rectangle {
                Layout.fillWidth: true
                height: 1
                color: "#292f39"
            }

            ColumnLayout {
                Layout.alignment: Qt.AlignHCenter
                spacing: 10
                Label {
                    id: timerLabel
                    Layout.alignment: Qt.AlignHCenter
                    text: "00:00:00"
                    color: "white"
                    font.pixelSize: 30
                    font.bold: true
                }
                RowLayout {
                    Layout.alignment: Qt.AlignHCenter
                    spacing: 10
                    Button {
                        text: stopwatchTimer.running ? "Pause" : "Start"
                        onClicked: {
                            if (stopwatchTimer.running) {
                                stopwatchTimer.stop()
                                heartRateManager.stopRecording()
                            } else {
                                stopwatchTimer.start()
                                heartRateManager.startRecording()
                        }
                    }
                }
                    Button {
                        text: "Reset"
                        onClicked: {
                            stopwatchTimer.stop()
                            heartRateManager.stopRecording()
                            stopwatchTime = 0
                            updateTimerLabel()
                    }
                }
            }
            }

                Timer {
                    id: stopwatchTimer
                    interval: 1000
                    repeat: true
                    onTriggered: {
                        stopwatchTime++
                        updateTimerLabel()
                    }
                }
            Label {
                Layout.alignment: Qt.AlignHCenter
                text: heartRateManager.connected ? "● Bluetooth connected" : "○ Bluetooth disconnected"
                color: heartRateManager.connected ? "#4caf50" : "#777f8c"
            font.pixelSize: 14
        }
    }
}
}

