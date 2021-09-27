#pragma once

#include "common/IdentityUnit.h"
#include "nodeeditor/nodeeditor_export.h"
#include <QGraphicsProxyWidget>

namespace QtNodes
{
class NODEEDITOR_EXPORT DangerousProxyWidget : public QGraphicsProxyWidget, public IdentityUnit
{
public:
    DangerousProxyWidget(QGraphicsItem* parent = nullptr, Qt::WindowFlags wFlags = nullptr,
                         const QUuid &id = QUuid::createUuid());

    ~DangerousProxyWidget() override;

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
};
} // namespace QtNodes
