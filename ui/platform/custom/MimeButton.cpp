#include "MimeButton.hpp"

#include "nodeeditor/models/MimeGenerator.hpp"
#include <QDebug>
#include <QMouseEvent>

namespace dscontrol
{
MimeButton::MimeButton(std::function<void(QObject*)> callback, QObject* parent)
: QObject {parent}
, callback_ {callback}
{
}

bool MimeButton::eventFilter(QObject* obj, QEvent* event)
{
    Q_UNUSED(obj);
    #ifdef WIN32
    int eventType = QEvent::Type::MouseButtonPress;
    bool retval = true;
    #else
    int eventType = QEvent::Type::MouseButtonRelease;
    bool retval = false;
    #endif
    if (callback_ && event && event->type() == eventType)
    {
        callback_(obj);
        return retval;
    }
    return false;
}
} // namespace dscontrol
