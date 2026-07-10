#include "FileOperations.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QUrl>

bool FileOperations::mkdir(const QString &parentPath, const QString &name)
{
    if (name.isEmpty())
        return false;
    QDir dir(parentPath);
    return dir.mkdir(name);
}

bool FileOperations::remove(const QString &path, bool isDir)
{
    if (isDir) {
        QDir dir(path);
        return dir.removeRecursively();
    }
    return QFile::remove(path);
}

bool FileOperations::rename(const QString &oldPath, const QString &newName)
{
    if (newName.isEmpty())
        return false;
    QFileInfo info(oldPath);
    const QString newPath = info.absoluteDir().filePath(newName);
    return QFile::rename(oldPath, newPath);
}

QString FileOperations::urlToPath(const QUrl &url) const
{
    return url.toLocalFile();
}

QUrl FileOperations::pathToUrl(const QString &path) const
{
    return QUrl::fromLocalFile(path);
}

QString FileOperations::homePath() const
{
    return QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
}

QString FileOperations::documentsPath() const
{
    return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
}

QString FileOperations::desktopPath() const
{
    return QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
}

QString FileOperations::parentPath(const QString &path) const
{
    QDir dir(path);
    if (!dir.cdUp())
        return QString(); // already at a filesystem root
    return dir.absolutePath();
}
