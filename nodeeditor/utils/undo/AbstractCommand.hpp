#pragma once

#include "nodes/Serializable.hpp"

#include <QJsonObject>
#include <QUndoCommand>
#include <QUuid>

namespace QtNodes
{
class AbstractCommand : public QUndoCommand
{
public:
    AbstractCommand(QUndoCommand* parent = nullptr);
    virtual ~AbstractCommand() override;

    virtual void undo() override = 0;
    virtual void redo() override = 0;

    virtual QUuid taskId() const;
    virtual QJsonObject itemState() const;

    virtual void setItemState(const QJsonObject &itemState);

    virtual bool isValid() const;

protected:
    QUuid id_;
    QJsonObject itemState_;
    bool valid_;
};
} // namespace QtNodes
