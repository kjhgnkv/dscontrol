#pragma once

#include <QAction>
#include <QUuid>

namespace QtNodes
{
class AbstractModelCommand;
class CommandFactory;
class CommandStack;
class AbstractCommand;

class UndoController
{
public:
    static UndoController &Instance();

    CommandStack* stack();
    const CommandFactory* factory() const;

    void push(QtNodes::AbstractCommand* command);

    QUuid lastTaskId() const;
    QAction* getUndoAction(QObject* parent);
    QAction* getRedoAction(QObject* parent);

    void begin(const QString &text);
    void end();

    bool isMacroOpen() const;

    void clear();

    std::chrono::microseconds time_;

private:
    UndoController();
    UndoController(const UndoController &) = delete;
    UndoController(UndoController &&) = delete;
    UndoController &operator=(const UndoController &) = delete;
    UndoController &operator=(UndoController &&) = delete;

    CommandStack* stack_;
    CommandFactory* factory_;

    bool isMacroOpen_;
};

} // namespace QtNodes
