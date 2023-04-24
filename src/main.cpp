#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <QDir>

#include "FileReaderWriter.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);

    engine.load(url);

    QString appDirPath;
    if (argc > 1)
        appDirPath = argv[1];

    if (appDirPath.isEmpty() || !QDir(appDirPath).exists())
        appDirPath = app.applicationDirPath();

    qInfo() << appDirPath;


    FileReaderWriter fileController;

    fileController.readFile("d:\\pocketbook-test-task\\images\\2_qimage.bmp");






    return app.exec();
}
