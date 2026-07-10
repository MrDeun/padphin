#include "FileOperations.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);

  QQmlApplicationEngine engine;

  qmlRegisterType<FileOperations>("FileManager", 1, 0, "FileOperations");

  // Matches the URI set in qt_add_qml_module() in CMakeLists.txt.
  // Resource path mirrors the source path given to QML_FILES, so
  // qml/Main.qml (on disk) -> qrc:/qt/qml/FileManager/qml/Main.qml.
  const QUrl url(QStringLiteral("qrc:/Main.qml"));
  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
  engine.load(url);

  if (engine.rootObjects().isEmpty())
    return -1;

  return app.exec();
}
