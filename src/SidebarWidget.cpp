#include "SidebarWidget.h"

#include <QVBoxLayout>

SidebarWidget::SidebarWidget(QWidget *parent)
    : QWidget(parent)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(6, 6, 6, 6);

    m_list = new QListWidget(this);
    m_list->setSpacing(2);
    m_list->setFrameShape(QFrame::NoFrame);

    connect(m_list, &QListWidget::currentRowChanged, this, [this](int row) {
        if (row >= 0 && row < m_places.size())
            emit folderRequested(m_places[row].path);
    });

    layout->addWidget(m_list);
}

void SidebarWidget::setPlaces(const QList<SidebarPlace> &places)
{
    m_places = places;
    m_list->clear();
    for (const auto &p : places)
        m_list->addItem(p.label);
    if (m_list->count() > 0)
        m_list->setCurrentRow(0);
}

int SidebarWidget::currentIndex() const
{
    return m_list->currentRow();
}

void SidebarWidget::setCurrentIndex(int index)
{
    if (index >= 0 && index < m_list->count())
        m_list->setCurrentRow(index);
}

void SidebarWidget::navigateUp()
{
    int row = m_list->currentRow();
    if (row > 0)
        m_list->setCurrentRow(row - 1);
}

void SidebarWidget::navigateDown()
{
    int row = m_list->currentRow();
    if (row < m_list->count() - 1)
        m_list->setCurrentRow(row + 1);
    else if (row < 0 && m_list->count() > 0)
        m_list->setCurrentRow(0);
}

void SidebarWidget::activateCurrent()
{
    int row = m_list->currentRow();
    if (row >= 0 && row < m_places.size())
        emit folderRequested(m_places[row].path);
}
