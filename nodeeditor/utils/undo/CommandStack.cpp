#include "utils/undo/CommandStack.hpp"
#include "utils/undo/AbstractModelCommand.hpp"

namespace QtNodes
{
CommandStack::CommandStack(QObject* parent)
: QUndoStack(parent)
{
    setUndoLimit(50);
}

QUuid CommandStack::lastTaskId() const
{
    if (auto task = dynamic_cast<const AbstractModelCommand*>(command(count() - 1)))
    {
        return task->taskId();
    }
    return {};
}

QAction* CommandStack::getUndoAction(QObject* parent)
{
    return createUndoAction(parent, tr("&Undo"));
}

QAction* CommandStack::getRedoAction(QObject* parent)
{
    return createRedoAction(parent, tr("&Redo"));
}

} // namespace QtNodes
