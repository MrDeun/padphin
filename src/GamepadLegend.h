#pragma once

#include <QWidget>
#include <QLabel>
#include <QString>
#include <QList>

class QHBoxLayout;

class GamepadLegend : public QWidget
{
    Q_OBJECT

public:
    explicit GamepadLegend(const QString &svgDir, QWidget *parent = nullptr);

private:
    struct Item {
        enum Kind { Text, Icon, Spacer };
        Kind kind;
        QString data;
    };

    QLabel *createIcon(const QString &name, int size = 18);
    QHBoxLayout *createRow(const QList<Item> &items);

    QString m_svgDir;
};
