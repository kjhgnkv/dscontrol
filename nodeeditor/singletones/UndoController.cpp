#include "singletones/UndoController.hpp"
#include "factories/CommandFactory.hpp"
#include "utils/undo/AbstractModelCommand.hpp"
#include "utils/undo/CommandStack.hpp"

namespace QtNodes
{
UndoController::UndoController()
{
    isMacroOpen_ = false;

    factory_ = new CommandFactory {};
    stack_ = factory_->createUndoStack(nullptr);
}

UndoController &UndoController::Instance()
{
    static UndoController undoController;
    return undoController;
}

CommandStack* UndoController::stack()
{
    return stack_;
}

const CommandFactory* UndoController::factory() const
{
    return factory_;
}

void UndoController::push(AbstractCommand* command)
{
    stack_->push(command);
}

QUuid UndoController::lastTaskId() const
{
    return stack_->lastTaskId();
}

QAction* UndoController::getUndoAction(QObject* parent)
{
    return stack_->getUndoAction(parent);
}

QAction* UndoController::getRedoAction(QObject* parent)
{
    return stack_->getRedoAction(parent);
}

void UndoController::begin(const QString &text)
{
    stack_->beginMacro(text);
    isMacroOpen_ = true;
}

void UndoController::end()
{
    stack_->endMacro();
    isMacroOpen_ = false;
}

bool UndoController::isMacroOpen() const
{
    return isMacroOpen_;
}

void UndoController::clear()
{
    stack_->clear();
}
} // namespace QtNodes
