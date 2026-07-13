#include "MainWindow.h"
#include "GamepadLegend.h"
#include "SidebarWidget.h"
#include "FileGridWidget.h"
#include "QGamepadManager.h"
#include "QGamepad.h"

#include <QToolBar>
#include <QSplitter>
#include <QStatusBar>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <QMouseEvent>

MainWindow::MainWindow(SdlGamepadManager *manager, QWidget *parent)
    : QMainWindow(parent)
    , m_manager(manager)
{
    setupUI();
    connectCurrentGamepad();

    connect(m_manager, &SdlGamepadManager::gamepadConnected,
            this, &MainWindow::onGamepadConnected);
    connect(m_manager, &SdlGamepadManager::gamepadDisconnected,
            this, &MainWindow::onGamepadDisconnected);

    m_stickRepeatTimer = new QTimer(this);
    m_stickRepeatTimer->setInterval(180);
    connect(m_stickRepeatTimer, &QTimer::timeout, this, &MainWindow::onStickInput);

    m_currentPath = m_fileOps.homePath();
    navigateTo(m_currentPath, false);
}

void MainWindow::setupUI()
{
    QToolBar *toolbar = new QToolBar("Navigation", this);
    toolbar->setMovable(false);

    QAction *backAction = toolbar->addAction(QStringLiteral("\u2190"));
    connect(backAction, &QAction::triggered, this, &MainWindow::goBack);

    QAction *forwardAction = toolbar->addAction(QStringLiteral("\u2192"));
    connect(forwardAction, &QAction::triggered, this, &MainWindow::goForward);

    QAction *upAction = toolbar->addAction(QStringLiteral("\u2191"));
    connect(upAction, &QAction::triggered, this, &MainWindow::goUp);

    toolbar->addSeparator();

    m_addressField = new QLineEdit(this);
    m_addressField->setPlaceholderText("Path");
    connect(m_addressField, &QLineEdit::returnPressed, this, &MainWindow::addressAccepted);

    QWidget *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    toolbar->addWidget(spacer);
    toolbar->addWidget(m_addressField);

    toolbar->addSeparator();

    QAction *newFolderAction = toolbar->addAction("New Folder");
    connect(newFolderAction, &QAction::triggered, this, &MainWindow::onNewFolder);

    m_renameAction = toolbar->addAction("Rename");
    m_renameAction->setEnabled(false);
    connect(m_renameAction, &QAction::triggered, this, &MainWindow::onRename);

    m_deleteAction = toolbar->addAction("Delete");
    m_deleteAction->setEnabled(false);
    connect(m_deleteAction, &QAction::triggered, this, &MainWindow::onDelete);

    addToolBar(toolbar);

    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);

    m_sidebar = new SidebarWidget(this);
    QList<SidebarPlace> places = {
        {"Home",     m_fileOps.homePath()},
        {"Desktop",  m_fileOps.desktopPath()},
        {"Documents", m_fileOps.documentsPath()}
    };
    m_sidebar->setPlaces(places);
    connect(m_sidebar, &SidebarWidget::folderRequested, this, [this](const QString &path) {
        m_focusedPanel = "sidebar";
        navigateTo(path);
    });

    m_fileGrid = new FileGridWidget(this);
    connect(m_fileGrid, &FileGridWidget::folderActivated, this, [this](const QString &path) {
        if (m_fileGrid->selectedIsDir())
            navigateTo(path);
    });
    connect(m_fileGrid, &FileGridWidget::selectionChanged,
            this, &MainWindow::onSelectionChanged);

    splitter->addWidget(m_sidebar);
    splitter->addWidget(m_fileGrid);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes({180, 820});

    setCentralWidget(splitter);

    m_statusLabel = new QLabel(m_currentPath, this);
    statusBar()->addWidget(m_statusLabel);

    {
        const QString subdir = QStringLiteral("/resources/Xbox Series/Vector");
        QStringList candidates = {
            QDir::currentPath() + subdir,
            QCoreApplication::applicationDirPath() + QStringLiteral("/../../../../") + subdir,
            QCoreApplication::applicationDirPath() + QStringLiteral("/..") + subdir,
            subdir.mid(1),
        };
        QString svgDir;
        for (const auto &c : candidates) {
            if (QDir(c).exists()) { svgDir = c; break; }
        }
        m_legend = new GamepadLegend(svgDir, this);
    }

    setMinimumSize(800, 500);
    resize(1000, 650);
    setWindowTitle("Padphin");
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    if (m_legend) {
        m_legend->adjustSize();
        m_legend->move(
            width() - m_legend->width() - 16,
            height() - m_legend->height() - statusBar()->height() - 16
        );
    }
}

void MainWindow::connectCurrentGamepad()
{
    if (m_pad) {
        disconnect(m_pad, nullptr, this, nullptr);
    }
    m_pad = m_manager->primaryGamepad();
    if (m_pad) {
        connect(m_pad, &SdlGamepad::buttonPressed, this, &MainWindow::onButtonPressed);
        connect(m_pad, &SdlGamepad::leftStickChanged, this, &MainWindow::onStickInput);
    }
    m_legend->setVisible(m_pad == nullptr || !m_pad->isConnected());
}

void MainWindow::navigateTo(const QString &path, bool recordHistory)
{
    if (path.isEmpty())
        return;

    m_currentPath = path;
    m_addressField->setText(path);
    m_fileGrid->setFolder(path);
    gridSelectionClear();

    if (recordHistory) {
        m_history = m_history.mid(0, m_historyIndex + 1);
        m_history.append(path);
        m_historyIndex = m_history.size() - 1;
    }
}

void MainWindow::goBack()
{
    if (m_historyIndex > 0) {
        m_historyIndex--;
        navigateTo(m_history[m_historyIndex], false);
    }
}

void MainWindow::goForward()
{
    if (m_historyIndex < m_history.size() - 1) {
        m_historyIndex++;
        navigateTo(m_history[m_historyIndex], false);
    }
}

void MainWindow::goUp()
{
    QString parent = m_fileOps.parentPath(m_currentPath);
    if (!parent.isEmpty())
        navigateTo(parent);
}

void MainWindow::addressAccepted()
{
    navigateTo(m_addressField->text());
}

void MainWindow::onNewFolder()
{
    bool ok;
    QString name = QInputDialog::getText(this, "New Folder", "Folder name:",
                                          QLineEdit::Normal, "New Folder", &ok);
    if (ok && !name.isEmpty()) {
        if (m_fileOps.mkdir(m_currentPath, name))
            refreshFolder();
    }
}

void MainWindow::onRename()
{
    if (m_selectedPath.isEmpty()) return;

    QFileInfo info(m_selectedPath);
    bool ok;
    QString newName = QInputDialog::getText(this, "Rename", "New name:",
                                            QLineEdit::Normal, info.fileName(), &ok);
    if (ok && !newName.isEmpty()) {
        if (m_fileOps.rename(m_selectedPath, newName)) {
            refreshFolder();
            gridSelectionClear();
        }
    }
}

void MainWindow::onDelete()
{
    if (m_selectedPath.isEmpty()) return;

    QFileInfo info(m_selectedPath);
    QString msg = QStringLiteral("Delete \"%1\"? This cannot be undone.").arg(info.fileName());
    auto btn = QMessageBox::question(this, "Delete", msg,
                                     QMessageBox::Yes | QMessageBox::No);
    if (btn == QMessageBox::Yes) {
        if (m_fileOps.remove(m_selectedPath, m_selectedIsDir)) {
            refreshFolder();
            gridSelectionClear();
        }
    }
}

void MainWindow::onSelectionChanged(const QString &path, bool isDir)
{
    m_selectedPath = path;
    m_selectedIsDir = isDir;
    m_statusLabel->setText(path.isEmpty() ? m_currentPath : path);
    m_renameAction->setEnabled(!path.isEmpty());
    m_deleteAction->setEnabled(!path.isEmpty());
}

void MainWindow::onGamepadConnected(SdlGamepad *)
{
    connectCurrentGamepad();
}

void MainWindow::onGamepadDisconnected(SdlGamepad *)
{
    connectCurrentGamepad();
}

void MainWindow::onButtonPressed(SdlGamepad::Button button)
{
    handleGamepadButton(button);
}

void MainWindow::onStickInput()
{
    if (!m_pad) { stopStickRepeat(); return; }

    float x = m_pad->leftStickX();
    float y = m_pad->leftStickY();
    const float threshold = 0.5f;

    if (qAbs(x) < threshold && qAbs(y) < threshold) {
        stopStickRepeat();
        return;
    }

    QString dir;
    if (qAbs(y) > qAbs(x))
        dir = y < 0 ? "up" : "down";
    else
        dir = x < 0 ? "left" : "right";

    if (m_focusedPanel == "sidebar" && (dir == "left" || dir == "right"))
        dir = y < 0 ? "up" : "down";

    if (dir != m_stickDir || !m_stickActive) {
        m_stickDir = dir;
        m_stickActive = true;
        doNavigate(dir);
        m_stickRepeatTimer->start();
    }
}

void MainWindow::stopStickRepeat()
{
    m_stickActive = false;
    m_stickDir.clear();
    m_stickRepeatTimer->stop();
}

void MainWindow::doNavigate(const QString &dir)
{
    if (m_focusedPanel == "grid") {
        if (dir == "up")    m_fileGrid->navigateUp();
        else if (dir == "down")  m_fileGrid->navigateDown();
        else if (dir == "left")  m_fileGrid->navigateLeft();
        else if (dir == "right") m_fileGrid->navigateRight();
    } else {
        if (dir == "up")   m_sidebar->navigateUp();
        else if (dir == "down") m_sidebar->navigateDown();
    }
}

void MainWindow::handleGamepadButton(SdlGamepad::Button button)
{
    if (!m_pad) return;

    switch (button) {
    case SdlGamepad::ButtonSouth:
        if (m_focusedPanel == "grid")
            m_fileGrid->activateCurrent();
        else
            m_sidebar->activateCurrent();
        break;
    case SdlGamepad::ButtonEast:
        goBack();
        break;
    case SdlGamepad::ButtonWest:
        onNewFolder();
        break;
    case SdlGamepad::ButtonNorth:
        if (!m_selectedPath.isEmpty())
            onRename();
        break;
    case SdlGamepad::ButtonStart:
        if (!m_selectedPath.isEmpty())
            onDelete();
        break;
    case SdlGamepad::ButtonBack:
        if (m_focusedPanel == "grid") {
            m_focusedPanel = "sidebar";
        } else {
            m_focusedPanel = "grid";
            m_fileGrid->forceFocus();
        }
        break;
    case SdlGamepad::ButtonLeftShoulder:
        goUp();
        break;
    case SdlGamepad::ButtonRightShoulder:
        goForward();
        break;
    case SdlGamepad::ButtonDpadDown:
        doNavigate("down");
        break;
    case SdlGamepad::ButtonDpadUp:
        doNavigate("up");
        break;
    case SdlGamepad::ButtonDpadLeft:
        if (m_focusedPanel == "grid")
            m_fileGrid->navigateLeft();
        break;
    case SdlGamepad::ButtonDpadRight:
        if (m_focusedPanel == "grid")
            m_fileGrid->navigateRight();
        break;
    default:
        break;
    }
}

void MainWindow::gridSelectionClear()
{
    m_fileGrid->clearSelection();
    m_selectedPath.clear();
    m_selectedIsDir = false;
}

void MainWindow::refreshFolder()
{
    QString current = m_currentPath;
    m_fileGrid->setFolder("");
    m_fileGrid->setFolder(current);
}
