#include "FileOperations.h"
#include "QGamepadManager.h"
#include "qlogging.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);
  SdlGamepadManager manager;

  qmlRegisterType<FileOperations>("FileManager", 1, 0, "FileOperations");
  qmlRegisterUncreatableType<SdlGamepad>("SdlQtGamepad", 1, 0, "SdlGamepad",
                                         "Created by SdlGamepadManager");
  qmlRegisterUncreatableType<SdlGamepadManager>(
      "SdlQtGamepad", 1, 0, "SdlGamepadManager",
      "Create one instance in C++ and expose it as a context property");

  if (!manager.start())
    qWarning("Gamepad manager failure...");

  QQmlApplicationEngine engine;
  engine.rootContext()->setContextProperty("GamepadManager", &manager);
  const QUrl url(QStringLiteral("qrc:/Main.qml"));
  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
  engine.load(url);

  if (engine.rootObjects().isEmpty())
    return -1;

  return app.exec();
}
