#include "utils/undo/RemoveNodeCommand.hpp"
#include "models/DataFlowModel.hpp"
#include "singletones/UndoController.hpp"
#include <QDebug>

namespace QtNodes
{
RemoveNodeCommand::RemoveNodeCommand(DataFlowModel* item, const NodeIndex &index, QtNodes::AbstractCommand* parent)
: AbstractModelCommand(item, parent)
, type_ {Item::NodeType::OtherType}
, deployType_ {Item::DeploymentType::None}
, index_ {index}
{
    type_ = item_->nodeType(index_);
    deployType_ = item->nodeDeploymentType(index_);
}

RemoveNodeCommand::~RemoveNodeCommand()
{
}

void RemoveNodeCommand::undo()
{
    auto start = std::chrono::high_resolution_clock::now();
    const auto &newIndex = item_->addNode({}, type_, deployType_, itemState_);
    if (newIndex.isValid())
    {
        index_ = newIndex;
    }
    auto stop = std::chrono::high_resolution_clock::now();

    UndoController::Instance().time_ += std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    qDebug() << "FUNCTION CALL TIME: " << std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();
    qDebug() << "FUNCTION FULL TIME: " << UndoController::Instance().time_.count();
}

void RemoveNodeCommand::redo()
{
    itemState_ = index_.nodeSave();
    value_ = item_->removeNodeCommand(index_);
}

std::pair<bool, std::map<QUuid, std::shared_ptr<Node>>::iterator> RemoveNodeCommand::value()
{
    return value_;
}

} // namespace QtNodes
