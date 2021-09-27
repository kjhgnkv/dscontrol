#include "utils/undo/AbstractSceneCommand.hpp"

QtNodes::AbstractSceneCommand::AbstractSceneCommand(const QUuid &sceneId, QUndoCommand* parent)
: AbstractCommand(parent)
, sceneId_ {sceneId}
, value_ {}
{
}

QtNodes::AbstractSceneCommand::~AbstractSceneCommand()
{
}

QUuid QtNodes::AbstractSceneCommand::sceneId() const
{
    return sceneId_;
}

QUuid QtNodes::AbstractSceneCommand::value() const
{
    return value_;
}
