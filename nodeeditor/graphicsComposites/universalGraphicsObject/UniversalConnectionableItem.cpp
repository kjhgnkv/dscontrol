#include "UniversalConnectionableItem.h"
#include <QDebug>

namespace QtNodes
{
UniversalConnectionableItem::UniversalConnectionableItem(const QUuid &id, QGraphicsItem* parentItem,
                                                         QObject* parentObject)
: UniversalMovableItem {id, parentItem, parentObject}
{
    setFlag(ItemSendsScenePositionChanges, true);
}

QVariant UniversalConnectionableItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    switch (change)
    {
        case ItemScenePositionHasChanged:
        {
            emit geometryChanged();
            break;
        }
        default:
        {
            break;
        }
    }
    return UniversalSelectableItem::itemChange(change, value);
}

} // namespace QtNodes
