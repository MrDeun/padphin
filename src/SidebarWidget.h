#pragma once

#include <QWidget>
#include <QListWidget>
#include <QList>

struct SidebarPlace {
    QString label;
    QString path;
};

class SidebarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SidebarWidget(QWidget *parent = nullptr);

    void setPlaces(const QList<SidebarPlace> &places);
    int currentIndex() const;
    void setCurrentIndex(int index);

    void navigateUp();
    void navigateDown();
    void activateCurrent();

signals:
    void folderRequested(const QString &path);

private:
    QListWidget *m_list;
    QList<SidebarPlace> m_places;
};
