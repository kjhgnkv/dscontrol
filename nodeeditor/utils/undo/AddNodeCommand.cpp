#include "utils/undo/AddNodeCommand.hpp"
#include "models/DataFlowModel.hpp"

using namespace QtNodes;

AddNodeCommand::AddNodeCommand(DataFlowModel* item, Item::NodeType type, Item::DeploymentType deployType,
                               AbstractCommand* parent)
: AbstractModelCommand {item, parent}
, type_ {type}
, deployType_ {deployType}
, index_ {}
{
    setText(QObject::tr("Add"));
}

void AddNodeCommand::undo()
{
    if (itemState_.empty())
    {
        itemState_ = index_.nodeSave();
    }

    if (item_)
    {
        item_->removeNodeCommand(index_);
    }
}

void AddNodeCommand::redo()
{
    if (item_ && item_->canAddNode(type_))
    {
        const auto &newIndex = item_->addNode(QUuid::createUuid(), type_, deployType_, itemState_);
        if (newIndex.isValid())
        {
            index_ = newIndex;
        }
    }
}

NodeIndex AddNodeCommand::index() const
{
    return index_;
}
