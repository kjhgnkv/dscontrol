#pragma once

#include "nodes/Serializable.hpp"
#include "AbstractCommand.hpp"
#include <QJsonObject>
#include <QUuid>

namespace QtNodes
{
class DataFlowModel;
class AbstractModelCommand : public AbstractCommand
{
public:
    AbstractModelCommand(DataFlowModel* item, QUndoCommand* parent = nullptr);
    virtual ~AbstractModelCommand() override;

    virtual DataFlowModel* item() const;

    virtual bool result() const;

protected:
    DataFlowModel* item_;
    bool result_;
};
} // namespace QtNodes
