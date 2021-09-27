#pragma once

#include "nodes/ConnectionID.hpp"
#include "common/nodeeditor_globals.hpp"

namespace QtNodes
{
class UniversalConnectionableItem;
class DataFlowModel;
class FlowScene;
class NodeIndex;

namespace Arrows
{
class AbstractArrow;
}

class AbstractModelCommand;
class AbstractSceneCommand;
class ArrowController;

class CommandStack;

class CommandFactory
{
public:
    explicit CommandFactory() = default;
    ~CommandFactory() = default;

    // AbstractCommandFactory interface
public:
    AbstractModelCommand* createAddNodeCommand(DataFlowModel* item, Item::NodeType type,
                                               Item::DeploymentType deployType,
                                               AbstractModelCommand* parent = nullptr) const;
    QtNodes::AbstractSceneCommand* createAddGraphicsFrameCommand(const QUuid &scene, const QPointF &pos,
                                                                 const QSizeF &size,
                                                                 AbstractSceneCommand* parent = nullptr) const;
    QtNodes::AbstractSceneCommand* createAddGraphicsTextCommand(const QUuid &scene, const QPointF &pos,
                                                                const QUuid &parentId = {},
                                                                AbstractSceneCommand* parent = nullptr) const;
    QtNodes::AbstractSceneCommand* createAddGraphicsArrowCommand(const QUuid &scene, const QUuid &source,
                                                                 const QUuid &dest, Arrows::AbstractArrow* arrow,
                                                                 AbstractSceneCommand* parent = nullptr) const;

    AbstractModelCommand* createRemoveNodeCommand(DataFlowModel* item, const NodeIndex &index,
                                                  AbstractModelCommand* parent = nullptr) const;

    AbstractSceneCommand* createRemoveGraphicsFrameCommand(const QUuid &scene, const QUuid &frameId,
                                                           AbstractSceneCommand* parent = nullptr) const;

    AbstractSceneCommand* createRemoveGraphicsTextCommand(const QUuid &scene, const QUuid &textId,
                                                          const QUuid &parentId = {},
                                                          AbstractSceneCommand* parent = nullptr) const;
    QtNodes::AbstractSceneCommand* createRemoveGraphicsArrowCommand(const QUuid &scene, const QUuid &arrowId,
                                                                    AbstractSceneCommand* parent = nullptr) const;

    AbstractSceneCommand* createChangeItemPropertyCommand(const QUuid &sceneId, const QUuid &itemId,
                                                          const Item::GraphicsItemProperties &property,
                                                          const QVariant &value, AbstractSceneCommand* parent) const;

    AbstractSceneCommand* createChangeItemPositionCommand(const QUuid &sceneId, const QUuid &itemId,
                                                          const QPointF &oldPos, const QPointF &newPos,
                                                          AbstractSceneCommand* parent) const;

    AbstractModelCommand* createAddConnectionCommand(DataFlowModel* item, const NodeIndex &lNodeIndex, const NodeIndex &sNodeIndex,
                                                     const NodeIndex &rNodeIdx, CommId commId, MsgId msgId,
                                                     AbstractModelCommand* parent = nullptr) const;

    AbstractModelCommand* createRemoveConnectionCommand(DataFlowModel* item, const ConnectionID &connId,
                                                        AbstractModelCommand* parent = nullptr) const;

    AbstractSceneCommand* createChangeItemSizeCommand(const QUuid &sceneId, const QUuid &itemId, const QSizeF &oldSize,
                                                      const QSizeF &newSize, AbstractSceneCommand* parent) const;

    CommandStack* createUndoStack(QObject* parent) const;
};

} // namespace QtNodes
