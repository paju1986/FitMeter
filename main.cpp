#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "HeartRateManager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    HeartRateManager heartRateManager;

    engine.rootContext()->setContextProperty(
        QStringLiteral("heartRateManager"),
        &heartRateManager
    );

    const QUrl url(
        QStringLiteral("qrc:/qt/qml/H9HeartRate/Main.qml")
    );

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() {
            QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection
    );

    engine.load(url);

    return app.exec();
}
