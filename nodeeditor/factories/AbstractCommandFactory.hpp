#pragma once

#include <QJsonObject>
#include <QUndoCommand>

namespace QtNodes
{
class AbstractModelCommand;
class CommandStack;

class Serializable;

class AbstractCommandFactory
{
public:
    explicit AbstractCommandFactory() = default;
    virtual ~AbstractCommandFactory() = default;

    virtual AbstractModelCommand* createNodeAddCommand(Serializable* item, const QJsonObject &pastSnap,
                                                       QUndoCommand* parent) const = 0;
    virtual AbstractModelCommand* createMoveCommand(Serializable* item, const QJsonObject &presentSnap,
                                                    const QJsonObject &pastSnap, QUndoCommand* parent) const = 0;
    virtual CommandStack* createUndoStack(QObject* parent) const = 0;
};

} // namespace QtNodes
