#include "utils/undo/AbstractCommand.hpp"

QtNodes::AbstractCommand::AbstractCommand(QUndoCommand* parent)
: QUndoCommand(parent)
, id_ {QUuid::createUuid()}
, itemState_ {}
{
    valid_ = true;
}

QtNodes::AbstractCommand::~AbstractCommand()
{
}

QUuid QtNodes::AbstractCommand::taskId() const
{
    return id_;
}

QJsonObject QtNodes::AbstractCommand::itemState() const
{
    return itemState_;
}

void QtNodes::AbstractCommand::setItemState(const QJsonObject &itemState)
{
    itemState_ = itemState;
}

bool QtNodes::AbstractCommand::isValid() const
{
    return valid_;
}
