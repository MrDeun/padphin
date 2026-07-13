#include "FileGridWidget.h"

#include <QVBoxLayout>
#include <QDir>
#include <QFileInfo>

FileGridWidget::FileGridWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);

    m_model = new QFileSystemModel(this);
    m_model->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    m_model->setRootPath(QDir::homePath());

    m_listView = new QListView(this);
    m_listView->setModel(m_model);
    m_listView->setViewMode(QListView::IconMode);
    m_listView->setIconSize(QSize(48, 48));
    m_listView->setGridSize(QSize(96, 96));
    m_listView->setMovement(QListView::Static);
    m_listView->setResizeMode(QListView::Adjust);
    m_listView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_listView->setWordWrap(true);
    m_listView->setSpacing(4);
    m_listView->setFlow(QListView::LeftToRight);
    m_listView->setWrapping(true);
    m_listView->setFrameShape(QFrame::NoFrame);

    connect(m_listView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &FileGridWidget::onCurrentChanged);
    connect(m_listView, &QListView::doubleClicked, this, &FileGridWidget::onDoubleClicked);
    connect(m_model, &QFileSystemModel::directoryLoaded, this, &FileGridWidget::onDirectoryLoaded);

    m_emptyLabel = new QLabel("This folder is empty", this);
    m_emptyLabel->setAlignment(Qt::AlignCenter);
    m_emptyLabel->setStyleSheet("color: #999; font-size: 14px;");
    m_emptyLabel->setVisible(false);

    layout->addWidget(m_listView);
}

void FileGridWidget::setFolder(const QString &path)
{
    m_folder = path;
    m_selectedPath.clear();
    m_selectedIsDir = false;
    m_listView->setRootIndex(m_model->setRootPath(path));

    // QFileSystemModel loads asynchronously — rowCount is 0 until
    // directoryLoaded fires.  Show the empty label only if the
    // directory was already cached (rowCount > 0 with 0 rows).
    m_listView->setVisible(true);
    m_emptyLabel->setVisible(false);
}

void FileGridWidget::onDirectoryLoaded(const QString &path)
{
    if (path != m_folder)
        return;

    QModelIndex root = m_model->setRootPath(path);
    bool empty = (m_model->rowCount(root) == 0);
    m_emptyLabel->setVisible(empty);
    m_listView->setVisible(!empty);

    if (!empty && m_listView->currentIndex().isValid())
        updateSelection(m_listView->currentIndex());
}

void FileGridWidget::onCurrentChanged(const QModelIndex &current)
{
    updateSelection(current);
}

void FileGridWidget::onDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) return;
    QString path = m_model->filePath(index);
    bool isDir = m_model->isDir(index);
    m_selectedPath = path;
    m_selectedIsDir = isDir;
    emit folderActivated(path);
}

void FileGridWidget::updateSelection(const QModelIndex &index)
{
    if (!index.isValid() || !m_folder.isEmpty() &&
        !index.data(QFileSystemModel::FilePathRole).toString().startsWith(m_folder)) {
        m_selectedPath.clear();
        m_selectedIsDir = false;
        emit selectionChanged("", false);
        return;
    }
    m_selectedPath = m_model->filePath(index);
    m_selectedIsDir = m_model->isDir(index);
    emit selectionChanged(m_selectedPath, m_selectedIsDir);
}

void FileGridWidget::navigateUp()
{
    QModelIndex current = m_listView->currentIndex();
    if (!current.isValid()) {
        if (m_model->rowCount(m_listView->rootIndex()) > 0) {
            QModelIndex first = m_model->index(0, 0, m_listView->rootIndex());
            m_listView->setCurrentIndex(first);
        }
        return;
    }
    int cols = qMax(1, (m_listView->viewport()->width()) / 96);
    QModelIndex prev = m_model->index(current.row() - cols, 0, m_listView->rootIndex());
    if (prev.isValid())
        m_listView->setCurrentIndex(prev);
    else if (current.row() > 0)
        m_listView->setCurrentIndex(m_model->index(0, 0, m_listView->rootIndex()));
}

void FileGridWidget::navigateDown()
{
    QModelIndex current = m_listView->currentIndex();
    int count = m_model->rowCount(m_listView->rootIndex());
    if (!current.isValid()) {
        if (count > 0) {
            QModelIndex first = m_model->index(0, 0, m_listView->rootIndex());
            m_listView->setCurrentIndex(first);
        }
        return;
    }
    int cols = qMax(1, (m_listView->viewport()->width()) / 96);
    QModelIndex next = m_model->index(current.row() + cols, 0, m_listView->rootIndex());
    if (next.isValid())
        m_listView->setCurrentIndex(next);
    else if (count > 0)
        m_listView->setCurrentIndex(m_model->index(count - 1, 0, m_listView->rootIndex()));
}

void FileGridWidget::navigateLeft()
{
    QModelIndex current = m_listView->currentIndex();
    if (!current.isValid() || current.row() <= 0) return;
    QModelIndex prev = m_model->index(current.row() - 1, 0, m_listView->rootIndex());
    if (prev.isValid())
        m_listView->setCurrentIndex(prev);
}

void FileGridWidget::navigateRight()
{
    QModelIndex current = m_listView->currentIndex();
    int count = m_model->rowCount(m_listView->rootIndex());
    if (!current.isValid() || current.row() >= count - 1) return;
    QModelIndex next = m_model->index(current.row() + 1, 0, m_listView->rootIndex());
    if (next.isValid())
        m_listView->setCurrentIndex(next);
}

void FileGridWidget::activateCurrent()
{
    QModelIndex current = m_listView->currentIndex();
    if (!current.isValid()) return;
    QString path = m_model->filePath(current);
    m_selectedPath = path;
    m_selectedIsDir = m_model->isDir(current);
    emit folderActivated(path);
}

void FileGridWidget::clearSelection()
{
    m_selectedPath.clear();
    m_selectedIsDir = false;
    m_listView->setCurrentIndex(QModelIndex());
    emit selectionChanged("", false);
}

void FileGridWidget::forceFocus()
{
    m_listView->setFocus();
}
