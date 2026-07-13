#pragma once

#include <QObject>
#include <QUrl>

class FileOperations : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

    // path is a local filesystem path (not a file:// URL) unless noted otherwise.
    Q_INVOKABLE bool mkdir(const QString &parentPath, const QString &name);
    Q_INVOKABLE bool remove(const QString &path, bool isDir);
    Q_INVOKABLE bool rename(const QString &oldPath, const QString &newName);

    // Convenience conversions QML can call directly instead of juggling
    // file:// URLs by hand.
    Q_INVOKABLE QString urlToPath(const QUrl &url) const;
    Q_INVOKABLE QUrl pathToUrl(const QString &path) const;

    Q_INVOKABLE QString homePath() const;
    Q_INVOKABLE QString documentsPath() const;
    Q_INVOKABLE QString desktopPath() const;

    // Returns the parent directory of `path`, or an empty string if
    // `path` is already a filesystem root.
    Q_INVOKABLE QString parentPath(const QString &path) const;
};
