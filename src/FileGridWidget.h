#pragma once

#include <QWidget>
#include <QListView>
#include <QFileSystemModel>
#include <QLabel>

class FileGridWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileGridWidget(QWidget *parent = nullptr);

    void setFolder(const QString &path);
    QString folder() const { return m_folder; }
    QString selectedPath() const { return m_selectedPath; }
    bool selectedIsDir() const { return m_selectedIsDir; }

    void navigateUp();
    void navigateDown();
    void navigateLeft();
    void navigateRight();
    void activateCurrent();
    void clearSelection();
    void forceFocus();

signals:
    void folderActivated(const QString &path);
    void selectionChanged(const QString &path, bool isDir);

private slots:
    void onCurrentChanged(const QModelIndex &current);
    void onDoubleClicked(const QModelIndex &index);
    void onDirectoryLoaded(const QString &path);

private:
    void updateSelection(const QModelIndex &index);

    QListView *m_listView;
    QFileSystemModel *m_model;
    QLabel *m_emptyLabel;
    QString m_selectedPath;
    bool m_selectedIsDir = false;
    QString m_folder;
};
