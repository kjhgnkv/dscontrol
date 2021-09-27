#include "RepoTabEventFilter.h"

#include "QDragEnterEvent"

using namespace dscontrol::CustomEventFilters;

RepoTabEventFilter::RepoTabEventFilter(QTabBar* bar)
: bar_ {bar}
{
}

bool RepoTabEventFilter::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::DragEnter)
    {
        auto ev = reinterpret_cast<QDragEnterEvent*>(event);
        int index = bar_->tabAt(ev->pos());
        bar_->setCurrentIndex(index);
    }
    return QObject::eventFilter(obj, event);
}
