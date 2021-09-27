#include "MimeMenu.hpp"

#include <QApplication>
#include <QDebug>
#include <QEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QToolBar>

namespace dscontrol
{
MimeMenu::MimeMenu(QToolBar* toolbar, QObject* parent)
: QObject {parent}
, toolbar_ {toolbar}
{
}

bool MimeMenu::eventFilter(QObject* object, QEvent* event)
{
    #ifdef WIN32
    int eventType = QEvent::Type::MouseButtonPress;
    bool retval = true;
    #else
    int eventType = QEvent::Type::MouseButtonRelease;
    bool retval = false;
    #endif
    if (toolbar_ && event && event->type() == eventType)
    {
        auto mouseEvent = dynamic_cast<QMouseEvent*>(event);
        auto menu = dynamic_cast<QMenu*>(object);
        if (mouseEvent && menu)
        {
            auto curAction = menu->actionAt(mouseEvent->pos());
            if (auto widget = toolbar_->widgetForAction(curAction))
            {
                QApplication::instance()->notify(widget, event);
                return retval;
            }
        }
    }
    return false;
}
} // namespace dscontrol
