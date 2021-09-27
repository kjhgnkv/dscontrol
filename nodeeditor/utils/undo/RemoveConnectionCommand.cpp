#include "RemoveConnectionCommand.hpp"

#include "nodes/ConnectionID.hpp"
#include "models/DataFlowModel.hpp"
#include "singletones/Facade.hpp"
#include <QDebug>

namespace QtNodes
{
RemoveConnectionCommand::RemoveConnectionCommand(QtNodes::DataFlowModel* item, const ConnectionID &connId,
                                                 QtNodes::AbstractCommand* parent)
: AbstractModelCommand(item, parent)
, lNodePtr_ {nullptr}
, rNodePtr_ {nullptr}
, sNodePtr_ {nullptr}
, commId_ {Invalid}
, msgId_ {Invalid}
, connId_ {connId}
{
    setText("RemoveConnectionCommand");

    auto connection = item_->connectionPointer(connId);
    if (!connection)
    {
        valid_ = false;
    }
    else
    {
        // TODO
        // itemState_ = connection->save();
    }
}

RemoveConnectionCommand::~RemoveConnectionCommand()
{
}

void RemoveConnectionCommand::undo()
{
    qDebug() << "Undo";
    if (connId_.isValid())
    {
        if (commId_ == Invalid)
        {
            commId_ = connId_.commId_;
        }
        if (msgId_ == Invalid)
        {
            msgId_ = connId_.msgId_;
        }

        if (!lNodePtr_)
        {
            lNodePtr_ = item_->nodePointer(item_->nodeIndex(connId_.lNodeID_));
        }
        if (!rNodePtr_)
        {
            rNodePtr_ = item_->nodePointer(item_->nodeIndex(connId_.rNodeID_));
        }
        if (!sNodePtr_)
        {
            sNodePtr_ = item_->nodePointer(item_->nodeIndex(connId_.sNodeID_));
        }

        item_->addConnection(lNodePtr_, rNodePtr_, sNodePtr_, itemState_);
    }
}

void RemoveConnectionCommand::redo()
{
    qDebug() << "Redo";

    result_ = item_->removeConnectionCommand(connId_);
}

} // namespace QtNodes
