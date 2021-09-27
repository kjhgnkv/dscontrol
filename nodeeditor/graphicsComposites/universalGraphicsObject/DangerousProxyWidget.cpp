#include "DangerousProxyWidget.h"

#include <QApplication>
#include <QDebug>
#include <QGraphicsSceneContextMenuEvent>
#include <QPainter>
#include <QPointer>
#include <QWidget>

namespace QtNodes
{
static inline bool bypassGraphicsProxyWidget(const QWidget* p)
{
    qDebug() << "DangerousProxyWidget: bypassGraphicsProxyWidget";
    while (p)
    {
        if (p->windowFlags() & Qt::BypassGraphicsProxyWidget)
            return true;
        p = p->parentWidget();
    }
    return false;
}

DangerousProxyWidget::DangerousProxyWidget(QGraphicsItem* parent, Qt::WindowFlags wFlags, const QUuid &id)
: QGraphicsProxyWidget(parent, wFlags)
, IdentityUnit(id)
{
    qDebug() << "DangerousProxyWidget:" << this->id() << "constructor";
}

DangerousProxyWidget::~DangerousProxyWidget()
{
    qDebug() << "DangerousProxyWidget:" << this->id() << "destructor";
}

void DangerousProxyWidget::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    qDebug() << "DangerousProxyWidget:" << this->id() << "contextMenuEvent";

    auto embeddedWidget = widget();
    if (!event || !embeddedWidget || !embeddedWidget->isVisible())
        return;

    /// Find widget position and receiver.
    QPointF pos = event->pos();
    QPointer<QWidget> alienWidget = embeddedWidget->childAt(pos.toPoint());

    QPointer<QWidget> receiver;
    if (nullptr != alienWidget)
    {
        receiver = alienWidget;
    }
    else
    {
        receiver = embeddedWidget;
    }

    /// Map event position from us to the receiver
    auto copyReceiver = receiver;
    while (copyReceiver && copyReceiver != embeddedWidget)
    {
        pos -= QPointF(copyReceiver->pos());
        copyReceiver = copyReceiver->parentWidget();
    }

    QPoint globalPos = receiver->mapToGlobal(pos.toPoint());

    if (bypassGraphicsProxyWidget(receiver))
    {
        globalPos = QCursor::pos();
    }
    QContextMenuEvent contextMenuEvent(QContextMenuEvent::Reason(event->reason()), pos.toPoint(), globalPos,
                                       event->modifiers());
    QApplication::sendEvent(receiver, &contextMenuEvent);
    event->setAccepted(contextMenuEvent.isAccepted());
}
} // namespace QtNodes
