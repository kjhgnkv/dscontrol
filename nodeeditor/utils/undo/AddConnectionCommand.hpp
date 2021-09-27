#pragma once

#include "nodes/ConnectionID.hpp"
#include "nodes/Node.hpp"
#include "nodes/NodeIndex.hpp"
#include "AbstractModelCommand.hpp"
#include <QPointer>

namespace QtNodes
{
class AddConnectionCommand : public AbstractModelCommand
{
public:
    AddConnectionCommand(DataFlowModel* item, const NodeIndex &lNodeIdx, const NodeIndex &rNodeIdx, const NodeIndex &sNodeIdx,
        CommId commId, MsgId msgId, AbstractCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    NodeIndex lNodeIdx_;
    NodeIndex rNodeIdx_;
    NodeIndex sNodeIdx_;
    QPointer<Node> lNodePtr_;
    QPointer<Node> rNodePtr_;
    QPointer<Node> sNodePtr_;
    CommId commId_;
    MsgId msgId_;
    ConnectionID connId_;
};
} // namespace QtNodes
