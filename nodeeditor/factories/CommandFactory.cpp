#include "CommandFactory.hpp"

#include "graphicsComposites/universalGraphicsObject/UniversalConnectionableItem.h"
#include "models/DataFlowModel.hpp"
#include "utils/undo/AddConnectionCommand.hpp"
#include "utils/undo/AddGraphicsArrowCommand.hpp"
#include "utils/undo/AddGraphicsFrameCommand.hpp"
#include "utils/undo/AddGraphicsTextCommand.hpp"
#include "utils/undo/AddNodeCommand.hpp"
#include "utils/undo/ChangeItemPositionCommand.hpp"
#include "utils/undo/ChangeItemPropertyCommand.hpp"
#include "utils/undo/ChangeItemSizeCommand.hpp"
#include "utils/undo/CommandStack.hpp"
#include "utils/undo/RemoveConnectionCommand.hpp"
#include "utils/undo/RemoveGraphicsArrowCommand.hpp"
#include "utils/undo/RemoveGraphicsFrameCommand.hpp"
#include "utils/undo/RemoveGraphicsTextCommand.hpp"
#include "utils/undo/RemoveNodeCommand.hpp"

namespace QtNodes
{
QtNodes::AbstractModelCommand* QtNodes::CommandFactory::createAddNodeCommand(DataFlowModel* item, Item::NodeType type,
                                                                             Item::DeploymentType deployType,
                                                                             AbstractModelCommand* parent) const
{
    return new AddNodeCommand(item, type, deployType, parent);
}

AbstractSceneCommand* CommandFactory::createAddGraphicsFrameCommand(const QUuid &scene, const QPointF &pos,
                                                                    const QSizeF &size,
                                                                    AbstractSceneCommand* parent) const
{
    return new AddGraphicsFrameCommand(scene, pos, size, parent);
}

AbstractSceneCommand* CommandFactory::createAddGraphicsTextCommand(const QUuid &scene, const QPointF &pos,
                                                                   const QUuid &parentId,
                                                                   AbstractSceneCommand* parent) const
{
    return new AddGraphicsTextCommand(scene, pos, parentId, parent);
}

AbstractSceneCommand* CommandFactory::createAddGraphicsArrowCommand(const QUuid &scene, const QUuid &source,
                                                                    const QUuid &dest, Arrows::AbstractArrow* arrow,
                                                                    AbstractSceneCommand* parent) const
{
    return new AddGraphicsArrowCommand(scene, source, dest, arrow, parent);
}

AbstractModelCommand* CommandFactory::createRemoveNodeCommand(DataFlowModel* item, const NodeIndex &index,
                                                              AbstractModelCommand* parent) const
{
    return new RemoveNodeCommand(item, index, parent);
}

AbstractSceneCommand* CommandFactory::createRemoveGraphicsFrameCommand(const QUuid &scene, const QUuid &frameId,
                                                                       AbstractSceneCommand* parent) const
{
    return new RemoveGraphicsFrameCommand(scene, frameId, parent);
}

AbstractSceneCommand* CommandFactory::createRemoveGraphicsTextCommand(const QUuid &scene, const QUuid &textId,
                                                                      const QUuid &parentId,
                                                                      AbstractSceneCommand* parent) const
{
    return new RemoveGraphicsTextCommand(scene, textId, parentId, parent);
}

AbstractSceneCommand* CommandFactory::createRemoveGraphicsArrowCommand(const QUuid &scene, const QUuid &arrowId,
                                                                       AbstractSceneCommand* parent) const
{
    return new RemoveGraphicsArrowCommand(scene, arrowId, parent);
}

AbstractSceneCommand* CommandFactory::createChangeItemPropertyCommand(const QUuid &sceneId, const QUuid &itemId,
                                                                      const Item::GraphicsItemProperties &property,
                                                                      const QVariant &value,
                                                                      AbstractSceneCommand* parent = nullptr) const
{
    return new ChangeItemPropertyCommand(sceneId, itemId, property, value, parent);
}

AbstractSceneCommand* CommandFactory::createChangeItemPositionCommand(const QUuid &sceneId, const QUuid &itemId,
                                                                      const QPointF &oldPos, const QPointF &newPos,
                                                                      AbstractSceneCommand* parent) const
{
    return new ChangeItemPositionCommand(sceneId, itemId, oldPos, newPos, parent);
}

AbstractModelCommand* CommandFactory::createAddConnectionCommand(DataFlowModel* item, const NodeIndex &lNodeIndex, const NodeIndex &rNodeIndex,
                                                                 const NodeIndex &sNodeIndex, CommId commId, MsgId msgId,
                                                                 AbstractModelCommand* parent) const
{
    return new AddConnectionCommand(item, lNodeIndex, rNodeIndex, sNodeIndex, commId, msgId, parent);
}

AbstractModelCommand* CommandFactory::createRemoveConnectionCommand(DataFlowModel* item, const ConnectionID &connId,
                                                                    AbstractModelCommand* parent) const
{
    return new RemoveConnectionCommand(item, connId, parent);
}

AbstractSceneCommand* CommandFactory::createChangeItemSizeCommand(const QUuid &sceneId, const QUuid &itemId,
                                                                  const QSizeF &oldSize, const QSizeF &newSize,
                                                                  AbstractSceneCommand* parent) const
{
    return new ChangeItemSizeCommand(sceneId, itemId, oldSize, newSize, parent);
}

CommandStack* QtNodes::CommandFactory::createUndoStack(QObject* parent) const
{
    return new CommandStack {parent};
}
} // namespace QtNodes
