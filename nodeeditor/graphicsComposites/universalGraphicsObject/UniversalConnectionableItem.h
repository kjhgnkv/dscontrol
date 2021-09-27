#pragma once

#include "UniversalMovableItem.h"
#include "nodeeditor/nodeeditor_export.h"
#include <QUuid>

namespace QtNodes
{
/*!
\brief Base class for items that can be connected by arrows (lines)
*/
class NODEEDITOR_EXPORT UniversalConnectionableItem : public UniversalMovableItem
{
Q_OBJECT

public:
    UniversalConnectionableItem(const QUuid &id = QUuid::createUuid(), QGraphicsItem* parentItem = nullptr,
                                QObject* parentObject = nullptr);

    virtual ~UniversalConnectionableItem() override = default;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

signals:
    /*!
    \brief emit this signal if you want lines connected to the item to be
    destroyed when items scene changed
    */
    void sceneHasChanged();

    /*!
    \brief emits when items geometry changes. Uses by arrows
    */
    void geometryChanged();
};
} // namespace QtNodes
