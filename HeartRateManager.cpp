#include "HeartRateManager.h"

#include <QDebug>

HeartRateManager::HeartRateManager(QObject *parent)
    : QObject(parent)
{
    m_discoveryAgent =
        new QBluetoothDeviceDiscoveryAgent(this);

    connect(
        m_discoveryAgent,
        &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
        this,
        &HeartRateManager::deviceDiscovered
    );

    connect(
        m_discoveryAgent,
        &QBluetoothDeviceDiscoveryAgent::finished,
        this,
        &HeartRateManager::scanFinished
    );

    connect(
        m_discoveryAgent,
        &QBluetoothDeviceDiscoveryAgent::canceled,
        this,
        &HeartRateManager::scanFinished
    );

    connect(
        m_discoveryAgent,
        &QBluetoothDeviceDiscoveryAgent::errorOccurred,
        this,
        &HeartRateManager::scanError
    );

    setStatus(QStringLiteral("Ready"));
}

HeartRateManager::~HeartRateManager()
{
    if (m_recordingStream) {
        delete m_recordingStream;
        m_recordingStream = nullptr;
    }
    if (m_recordingFile) {
        m_recordingFile->close();
        delete m_recordingFile;
        m_recordingFile = nullptr;
    }
    if (m_controller) {
        m_controller->disconnectFromDevice();
    }
}

int HeartRateManager::heartRate() const
{
    return m_heartRate;
}

bool HeartRateManager::scanning() const
{
    return m_scanning;
}

bool HeartRateManager::connected() const
{
    return m_connected;
}

bool HeartRateManager::recording() const
{
    return m_recording;
}

QString HeartRateManager::deviceName() const
{
    return m_deviceName;
}

QString HeartRateManager::status() const
{
    return m_status;
}

void HeartRateManager::setStatus(
    const QString &status)
{
    if (m_status == status)
        return;

    m_status = status;

    emit statusChanged();

    qDebug() << "Status:" << m_status;
}

void HeartRateManager::startScan()
{
    if (m_scanning)
        return;

    if (m_controller) {
        m_controller->disconnectFromDevice();
        m_controller->deleteLater();
        m_controller = nullptr;
    }

    m_heartRateService = nullptr;

    m_heartRate = 0;
    emit heartRateChanged();

    m_deviceName.clear();
    emit deviceNameChanged();

    setStatus(QStringLiteral("Scanning..."));

    m_scanning = true;
    emit scanningChanged();

    m_discoveryAgent->setLowEnergyDiscoveryTimeout(15000);

    m_discoveryAgent->start(
        QBluetoothDeviceDiscoveryAgent::LowEnergyMethod
    );
}

void HeartRateManager::stopScan()
{
    if (!m_scanning)
        return;

    m_discoveryAgent->stop();
}

void HeartRateManager::deviceDiscovered(
    const QBluetoothDeviceInfo &device)
{
    qDebug()
        << "BLE device:"
        << device.name()
        << device.address().toString();

    /*
     * Look for the standard Bluetooth Heart Rate Service.
     *
     * This is more reliable than checking whether the name
     * contains "H9".
     */
    if (!device.serviceUuids().contains(
            m_heartRateServiceUuid)) {
        return;
    }

    qDebug()
        << "Found heart-rate device:"
        << device.name();

    m_discoveryAgent->stop();

    connectToDevice(device);
}

void HeartRateManager::scanFinished()
{
    if (!m_scanning)
        return;

    m_scanning = false;
    emit scanningChanged();

    if (!m_controller) {
        setStatus(
            QStringLiteral(
                "No heart-rate sensor found"
            )
        );
    }
}

void HeartRateManager::scanError(
    QBluetoothDeviceDiscoveryAgent::Error error)
{
    Q_UNUSED(error);

    m_scanning = false;
    emit scanningChanged();

    setStatus(
        QStringLiteral("Bluetooth scan error: %1")
            .arg(m_discoveryAgent->errorString())
    );
}

void HeartRateManager::connectToDevice(
    const QBluetoothDeviceInfo &device)
{
    m_deviceName = device.name();

    if (m_deviceName.isEmpty()) {
        m_deviceName =
            QStringLiteral("Heart Rate Sensor");
    }

    emit deviceNameChanged();

    setStatus(
        QStringLiteral("Connecting to %1...")
            .arg(m_deviceName)
    );

    if (m_controller) {
        m_controller->deleteLater();
        m_controller = nullptr;
    }

    m_controller =
        QLowEnergyController::createCentral(
            device,
            this
        );

    connect(
        m_controller,
        &QLowEnergyController::connected,
        this,
        &HeartRateManager::controllerConnected
    );

    connect(
        m_controller,
        &QLowEnergyController::disconnected,
        this,
        &HeartRateManager::controllerDisconnected
    );

    connect(
        m_controller,
        &QLowEnergyController::errorOccurred,
        this,
        &HeartRateManager::controllerError
    );

    connect(
        m_controller,
        &QLowEnergyController::discoveryFinished,
        this,
        &HeartRateManager::discoveryFinished
    );

    m_controller->connectToDevice();
}

void HeartRateManager::controllerConnected()
{
    m_connected = true;
    emit connectedChanged();

    setStatus(
        QStringLiteral(
            "Connected - discovering services..."
        )
    );

    m_controller->discoverServices();
}

void HeartRateManager::controllerDisconnected()
{
    m_connected = false;
    emit connectedChanged();

    m_heartRateService = nullptr;

    setStatus(QStringLiteral("Disconnected"));

    m_heartRate = 0;
    emit heartRateChanged();
}

void HeartRateManager::controllerError(
    QLowEnergyController::Error error)
{
    Q_UNUSED(error);

    const QString errorText =
        m_controller
            ? m_controller->errorString()
            : QStringLiteral("Unknown error");

    qWarning() << "Bluetooth error:" << errorText;

    m_connected = false;
    emit connectedChanged();

    setStatus(
        QStringLiteral(
            "Bluetooth error: %1"
        ).arg(errorText)
    );
}

void HeartRateManager::discoveryFinished()
{
    if (!m_controller)
        return;

    qDebug() << "Service discovery finished";

    m_heartRateService =
        m_controller->createServiceObject(
            m_heartRateServiceUuid,
            this
        );

    if (!m_heartRateService) {
        setStatus(
            QStringLiteral(
                "Heart Rate Service not found"
            )
        );
        return;
    }

    connect(
        m_heartRateService,
        &QLowEnergyService::stateChanged,
        this,
        &HeartRateManager::serviceStateChanged
    );

    connect(
        m_heartRateService,
        &QLowEnergyService::characteristicChanged,
        this,
        &HeartRateManager::characteristicChanged
    );

    setStatus(
        QStringLiteral(
            "Discovering heart-rate characteristic..."
        )
    );

    m_heartRateService->discoverDetails();
}

void HeartRateManager::serviceStateChanged(
    QLowEnergyService::ServiceState state)
{
    /*
     * RemoteServiceDiscovered is the current Qt 6 name.
     *
     * ServiceDiscovered is retained as an alias by some Qt
     * versions, but RemoteServiceDiscovered avoids the
     * deprecation warning.
     */
    if (state !=
        QLowEnergyService::RemoteServiceDiscovered) {
        return;
    }

    qDebug() << "Heart Rate Service discovered";

    const QLowEnergyCharacteristic characteristic =
        m_heartRateService->characteristic(
            m_heartRateMeasurementUuid
        );

    if (!characteristic.isValid()) {
        setStatus(
            QStringLiteral(
                "Heart Rate Measurement not found"
            )
        );
        return;
    }

    const QLowEnergyDescriptor descriptor =
        characteristic.descriptor(
            QBluetoothUuid::DescriptorType::
                ClientCharacteristicConfiguration
        );

    if (!descriptor.isValid()) {
        setStatus(
            QStringLiteral(
                "Notification descriptor not found"
            )
        );
        return;
    }

    /*
     * 0x0100 enables notifications.
     */
    m_heartRateService->writeDescriptor(
        descriptor,
        QByteArray::fromHex("0100")
    );

    setStatus(
        QStringLiteral(
            "Connected - waiting for heart rate..."
        )
    );

    qDebug()
        << "Heart rate notifications enabled";
}

void HeartRateManager::characteristicChanged(
    const QLowEnergyCharacteristic &characteristic,
    const QByteArray &value)
{
    if (characteristic.uuid() !=
        m_heartRateMeasurementUuid) {
        return;
    }

    parseHeartRate(value);
}

void HeartRateManager::parseHeartRate(
    const QByteArray &data)
{
    if (data.size() < 2)
        return;

    const quint8 flags =
        static_cast<quint8>(data.at(0));

    /*
     * Bit 0:
     *
     * 0 = UINT8 heart rate
     * 1 = UINT16 heart rate
     */
    const bool is16Bit =
        (flags & 0x01) != 0;

    int bpm = 0;

    if (!is16Bit) {
        bpm =
            static_cast<quint8>(data.at(1));
    } else {
        if (data.size() < 3)
            return;

        const quint8 low =
            static_cast<quint8>(data.at(1));

        const quint8 high =
            static_cast<quint8>(data.at(2));

        bpm =
            static_cast<int>(
                low | (high << 8)
            );
    }

    if (bpm <= 0 || bpm > 250)
        return;

    if (m_heartRate == bpm)
        return;

    m_heartRate = bpm;

    if (m_recording && m_recordingStream) {
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        *m_recordingStream << timestamp << " - Heart Rate: " << bpm << " BPM\n";
        m_recordingStream->flush();
    }

    emit heartRateChanged();

    setStatus(
        QStringLiteral(
            "Receiving heart rate"
        )
    );

    qDebug()
        << "Heart rate:"
        << bpm
        << "BPM";
}

void HeartRateManager::disconnectDevice()
{
    if (!m_controller)
        return;

    m_controller->disconnectFromDevice();
}

void HeartRateManager::startRecording()
{
    if (m_recording)
        return;

    QString fileName = QString("hr_%1.txt")
            .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));

    m_recordingFile = new QFile(fileName, this);
    if (!m_recordingFile->open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Could not open file for recording:" << fileName;
        delete m_recordingFile;
        m_recordingFile = nullptr;
        return;
    }

    m_recordingStream = new QTextStream(m_recordingFile);
    *m_recordingStream << "Heart Rate Recording - " << QDateTime::currentDateTime().toString() << "\n";
    *m_recordingStream << "--------------------------------------------------\n";

    m_recording = true;
    emit recordingChanged();
    setStatus(QStringLiteral("Recording started..."));
    qDebug() << "Recording started to file:" << fileName;
}

void HeartRateManager::stopRecording()
{
    if (!m_recording)
        return;

    if (m_recordingStream) {
        *m_recordingStream << "--------------------------------------------------\n";
        *m_recordingStream << "Recording stopped at " << QDateTime::currentDateTime().toString() << "\n";
        delete m_recordingStream;
        m_recordingStream = nullptr;
    }

    if (m_recordingFile) {
        m_recordingFile->close();
        delete m_recordingFile;
        m_recordingFile = nullptr;
    }

    m_recording = false;
    emit recordingChanged();
    setStatus(QStringLiteral("Recording stopped"));
    qDebug() << "Recording stopped";
}
