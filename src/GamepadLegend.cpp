#include "GamepadLegend.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSvgRenderer>
#include <QPixmap>
#include <QPainter>
#include <QFileInfo>
#include <QDir>

GamepadLegend::GamepadLegend(const QString &svgDir, QWidget *parent)
    : QWidget(parent)
    , m_svgDir(svgDir)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setStyleSheet(
        "background: transparent;"
    );

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(14, 10, 14, 10);
    mainLayout->setSpacing(3);

#define ICON(n)  GamepadLegend::Item{GamepadLegend::Item::Icon,   QStringLiteral(n)}
#define TEXT(t)  GamepadLegend::Item{GamepadLegend::Item::Text,   QStringLiteral(t)}
#define SEP      GamepadLegend::Item{GamepadLegend::Item::Spacer, {}}

    mainLayout->addLayout(createRow({ICON("xbox_button_a"),      TEXT("Open"),
                                     SEP,
                                     ICON("xbox_button_b"),      TEXT("Back")}));

    mainLayout->addLayout(createRow({ICON("xbox_button_x"),      TEXT("New Folder"),
                                     SEP,
                                     ICON("xbox_button_y"),      TEXT("Rename")}));

    mainLayout->addLayout(createRow({ICON("xbox_button_start"),  TEXT("Delete"),
                                     SEP,
                                     ICON("xbox_button_back"),   TEXT("Focus")}));

    mainLayout->addLayout(createRow({ICON("xbox_lb"),            TEXT("Up"),
                                     SEP,
                                     ICON("xbox_rb"),            TEXT("Forward")}));

    mainLayout->addLayout(createRow({ICON("xbox_dpad_all"),
                                     TEXT(" / "),
                                     ICON("xbox_stick_l"),
                                     TEXT("Navigate")}));
}

QLabel *GamepadLegend::createIcon(const QString &name, int size)
{
    QLabel *label = new QLabel(this);
    QPixmap pm(size, size);
    pm.fill(Qt::transparent);

    QString svgPath = m_svgDir + QDir::separator() + name + ".svg";
    if (QFileInfo::exists(svgPath)) {
        QSvgRenderer renderer(svgPath);
        QPainter painter(&pm);
        renderer.render(&painter, QRectF(0, 0, size, size));
        painter.end();
    }

    label->setPixmap(pm);
    return label;
}

QHBoxLayout *GamepadLegend::createRow(const QList<Item> &items)
{
    auto *row = new QHBoxLayout;
    row->setSpacing(4);

    for (const auto &item : items) {
        switch (item.kind) {
        case Item::Icon:
            row->addWidget(createIcon(item.data));
            break;
        case Item::Text: {
            auto *l = new QLabel(item.data, this);
            l->setStyleSheet("color: white; font-size: 10px;");
            row->addWidget(l);
            break;
        }
        case Item::Spacer: {
            auto *l = new QLabel(QStringLiteral("|"), this);
            l->setStyleSheet("color: rgba(255,255,255,160); font-size: 10px;");
            row->addWidget(l);
            break;
        }
        }
    }
    row->addStretch();
    return row;
}
