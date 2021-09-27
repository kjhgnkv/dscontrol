#pragma once

#include "nodes/ConnectionID.hpp"
#include "nodes/Node.hpp"
#include "nodes/NodeIndex.hpp"
#include "utils/undo/AbstractModelCommand.hpp"
#include <QPointer>

namespace QtNodes
{
class RemoveConnectionCommand : public AbstractModelCommand
{
public:
    RemoveConnectionCommand(DataFlowModel* item, const ConnectionID &connId, AbstractCommand* parent = nullptr);
    virtual ~RemoveConnectionCommand() override;

    void undo() override;
    void redo() override;

private:
    QtNodes::NodeIndex lNodeIdx_;
    QtNodes::NodeIndex rNodeIdx_;
    QtNodes::NodeIndex sNodeIdx_;
    QPointer<Node> lNodePtr_;
    QPointer<Node> rNodePtr_;
    QPointer<Node> sNodePtr_;

    QtNodes::CommId commId_;
    MsgId msgId_;

    ConnectionID connId_;
};
} // namespace QtNodes
