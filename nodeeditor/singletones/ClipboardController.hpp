#pragma once

#include "nodes/ConnectionID.hpp"
#include "nodes/NodeIndex.hpp"

#include <QObject>
#include <list>

class QMimeData;
class QAction;

namespace QtNodes
{
class UniversalSelectableItem;

struct CutData
{
    NodeIndex idx_;
    std::map<QUuid, ConnectionID> connsToRestore_;
    UniversalSelectableItem* item_;
};

class ClipboardController : public QObject
{
Q_OBJECT
public:
    static ClipboardController &Instance();

private:
    ClipboardController();
    ClipboardController(const ClipboardController &) = delete;
    ClipboardController(ClipboardController &&) = delete;
    ClipboardController &operator=(const ClipboardController &) = delete;
    ClipboardController &operator=(ClipboardController &&) = delete;

public:
    QMimeData* copyBuffer() const;
    std::list<CutData> cutBuffer() const;

    QAction* copyAction() const;
    QAction* cutAction() const;
    QAction* pasteAction() const;

private slots:
    void copy();
    void cut();
    void paste();

private:
    QAction* copyAction_;
    QAction* cutAction_;
    QAction* pasteAction_;

    QMimeData* copyBuffer_;
    std::list<CutData> cutBuffer_;
};

} // namespace QtNodes
