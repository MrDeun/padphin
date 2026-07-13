#include "MainWindow.h"
#include "QGamepadManager.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    SdlGamepadManager manager;
    if (!manager.start())
        qWarning("Gamepad manager failure...");

    MainWindow window(&manager);
    window.show();

    return app.exec();
}
