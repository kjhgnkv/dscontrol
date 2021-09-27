#pragma once

#include <QJsonObject>
#include <QUuid>

#include "AbstractCommand.hpp"
#include "nodes/Serializable.hpp"

namespace QtNodes
{
class FlowScene;

class AbstractSceneCommand : public AbstractCommand
{
public:
    explicit AbstractSceneCommand(const QUuid &sceneId, QUndoCommand* parent = nullptr);
    virtual ~AbstractSceneCommand();
    virtual QUuid sceneId() const;

    virtual QUuid value() const;

protected:
    QUuid sceneId_;
    QUuid value_;
};

} // namespace QtNodes
