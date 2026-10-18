#pragma once

#include <QObject>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QLowEnergyController>
#include <QLowEnergyService>
#include <QBluetoothDeviceInfo>
#include <QBluetoothUuid>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

class HeartRateManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int heartRate READ heartRate NOTIFY heartRateChanged)
    Q_PROPERTY(bool scanning READ scanning NOTIFY scanningChanged)
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(QString deviceName READ deviceName NOTIFY deviceNameChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(bool recording READ recording NOTIFY recordingChanged)

public:
    explicit HeartRateManager(QObject *parent = nullptr);
    ~HeartRateManager();

    int heartRate() const;
    bool scanning() const;
    bool connected() const;
    bool recording() const;
    QString deviceName() const;
    QString status() const;

    Q_INVOKABLE void startScan();
    Q_INVOKABLE void stopScan();
    Q_INVOKABLE void disconnectDevice();
    Q_INVOKABLE void startRecording();
    Q_INVOKABLE void stopRecording();

signals:
    void heartRateChanged();
    void scanningChanged();
    void connectedChanged();
    void recordingChanged();
    void deviceNameChanged();
    void statusChanged();

private slots:
    void deviceDiscovered(const QBluetoothDeviceInfo &device);
    void scanFinished();
    void scanError(QBluetoothDeviceDiscoveryAgent::Error error);

    void controllerConnected();
    void controllerDisconnected();
    void controllerError(QLowEnergyController::Error error);

    void discoveryFinished();

    void serviceStateChanged(
        QLowEnergyService::ServiceState state
    );

    void characteristicChanged(
        const QLowEnergyCharacteristic &characteristic,
        const QByteArray &value
    );

private:
    void setStatus(const QString &status);

    void connectToDevice(
        const QBluetoothDeviceInfo &device
    );

    void parseHeartRate(
        const QByteArray &data
    );

    QBluetoothDeviceDiscoveryAgent *m_discoveryAgent = nullptr;

    QLowEnergyController *m_controller = nullptr;

    QLowEnergyService *m_heartRateService = nullptr;

    int m_heartRate = 0;

    bool m_scanning = false;
    bool m_connected = false;
    bool m_recording = false;

    QString m_deviceName;
    QString m_status;

    QFile *m_recordingFile = nullptr;
    QTextStream *m_recordingStream = nullptr;

    const QBluetoothUuid m_heartRateServiceUuid =
        QBluetoothUuid(
            QStringLiteral(
                "0000180d-0000-1000-8000-00805f9b34fb"
            )
        );

    const QBluetoothUuid m_heartRateMeasurementUuid =
        QBluetoothUuid(
            QStringLiteral(
                "00002a37-0000-1000-8000-00805f9b34fb"
            )
        );
};

