#include "utils/undo/AbstractModelCommand.hpp"

namespace QtNodes
{
AbstractModelCommand::AbstractModelCommand(DataFlowModel* item, QUndoCommand* parent)

: AbstractCommand(parent)
, item_ {item}
{
}

AbstractModelCommand::~AbstractModelCommand()
{
}

DataFlowModel* AbstractModelCommand::item() const
{
    return item_;
}

bool AbstractModelCommand::result() const
{
    return result_;
}
} // namespace QtNodes
