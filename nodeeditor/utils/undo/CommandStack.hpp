#pragma once

#include <QUndoStack>
#include <QUuid>

namespace QtNodes
{
class CommandStack : public QUndoStack
{
public:
    explicit CommandStack(QObject* parent = nullptr);

    QUuid lastTaskId() const;
    QAction* getUndoAction(QObject* parent);
    QAction* getRedoAction(QObject* parent);
};
} // namespace QtNodes
