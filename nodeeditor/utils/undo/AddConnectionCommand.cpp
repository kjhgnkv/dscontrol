#include "AddConnectionCommand.hpp"

#include "nodes/ConnectionID.hpp"
#include "models/DataFlowModel.hpp"
#include "singletones/Facade.hpp"

using namespace QtNodes;

AddConnectionCommand::AddConnectionCommand(DataFlowModel* item, const NodeIndex &lNodeIdx, const NodeIndex &rNodeIdx,
                                           const NodeIndex &sNodeIdx, CommId commId, MsgId msgId, AbstractCommand* parent)
: AbstractModelCommand {item, parent}
, lNodeIdx_ {lNodeIdx}
, rNodeIdx_(rNodeIdx)
, sNodeIdx_(sNodeIdx)
, lNodePtr_ {nullptr}
, rNodePtr_ {nullptr}
, sNodePtr_ {nullptr}
, commId_ {commId}
, msgId_ {msgId}
, connId_ {}
{
    setText("AddConnectionCommand");
}

void AddConnectionCommand::undo()
{
    if (connId_.isValid())
    {
        result_ = item_->removeConnectionCommand(connId_);
    }
}

void AddConnectionCommand::redo()
{
    if (!lNodePtr_)
    {
        lNodePtr_ = item_->nodePointer(lNodeIdx_);
    }
    if (!rNodePtr_)
    {
        rNodePtr_ = item_->nodePointer(rNodeIdx_);
    }
    if (!sNodePtr_)
    {
        sNodePtr_ = item_->nodePointer(sNodeIdx_);
    }

    if (lNodePtr_ && rNodePtr_)
    {
        if (!itemState_.isEmpty())
        {
            connId_ = item_->addConnection(lNodePtr_, rNodePtr_, sNodePtr_, itemState_);
        }
        else
        {
            connId_ = item_->addConnection(lNodePtr_, rNodePtr_, sNodePtr_, commId_, msgId_);

            // TODO
            // itemState_ = item_->connectionPointer(connId_)->save();
        }
        result_ = connId_.isValid();
    }
}
