#include "DSTabBar.hpp"

#include <QMouseEvent>

namespace dscontrol
{
DSTabBar::DSTabBar(QWidget* parent)
: QTabBar(parent)
, tabRect_ {}
, delta_ {}
, currentTab_ {}
{
    setMovable(true);
}

void DSTabBar::mousePressEvent(QMouseEvent* event)
{
    auto pos = event->pos();
    auto index = tabAt(pos);
    currentTab_ = index;

    if (currentTab_ == 0)
    {
        setMovable(false);
    }

    delta_ = pos.x() - mapToParent(tabRect(currentTab_).topLeft()).x();

    QTabBar::mousePressEvent(event);
}

void DSTabBar::mouseMoveEvent(QMouseEvent* event)
{
    if (currentTab_ != 0 && event->pos().x() - delta_ - 2 > mapToParent(tabRect(0).topRight()).x())
    {
        setMovable(true);
    }
    else
    {
        setMovable(false);
    }

    QTabBar::mouseMoveEvent(event);
}

void DSTabBar::mouseReleaseEvent(QMouseEvent* event)
{
    setMovable(true);

    QTabBar::mouseReleaseEvent(event);
}

} // namespace dscontrol
