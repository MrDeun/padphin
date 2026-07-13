#pragma once

#include <QMainWindow>
#include <QLineEdit>
#include <QLabel>
#include <QStringList>
#include <QTimer>
#include <QAction>
#include <QResizeEvent>

#include "FileOperations.h"
#include "QGamepad.h"
#include "QGamepadManager.h"
class SidebarWidget;
class FileGridWidget;
class GamepadLegend;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(SdlGamepadManager *manager, QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void navigateTo(const QString &path, bool recordHistory = true);
    void goBack();
    void goForward();
    void goUp();
    void onNewFolder();
    void onRename();
    void onDelete();
    void addressAccepted();
    void onSelectionChanged(const QString &path, bool isDir);
    void onGamepadConnected(SdlGamepad *gp);
    void onGamepadDisconnected(SdlGamepad *gp);
    void onButtonPressed(SdlGamepad::Button button);
    void onStickInput();

private:
    void setupUI();
    void connectCurrentGamepad();
    void gridSelectionClear();
    void handleGamepadButton(SdlGamepad::Button button);
    void doNavigate(const QString &dir);
    void stopStickRepeat();
    void refreshFolder();

    FileOperations m_fileOps;
    SdlGamepadManager *m_manager;
    SdlGamepad *m_pad = nullptr;

    QLineEdit *m_addressField;
    SidebarWidget *m_sidebar;
    FileGridWidget *m_fileGrid;
    QLabel *m_statusLabel;
    GamepadLegend *m_legend;
    QAction *m_renameAction;
    QAction *m_deleteAction;

    QStringList m_history;
    int m_historyIndex = -1;
    QString m_currentPath;

    QString m_focusedPanel = "grid";
    QString m_stickDir;
    bool m_stickActive = false;
    QTimer *m_stickRepeatTimer;

    QString m_selectedPath;
    bool m_selectedIsDir = false;
};
