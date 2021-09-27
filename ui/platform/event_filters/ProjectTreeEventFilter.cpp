#include "ProjectTreeEventFilter.h"

#include <QKeyEvent>
#include "manager/models/ProjectModel.hpp"

using namespace dscontrol::CustomEventFilters;

ProjectTreeEventFilter::ProjectTreeEventFilter(QTreeView* tree, QObject* parent)
: QObject(parent)
, tree_ {tree}
{
}

bool ProjectTreeEventFilter::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent* key = static_cast<QKeyEvent*>(event);
        if (key->key() == Qt::Key_Delete)
        {
            auto index = tree_->currentIndex();
            if (index.isValid())
            {
                auto id = index.data(ProjectModel::Roles::Uid).toUuid();
                emit delPressed(id);
            }
        }
    }
    if (event->type() == QEvent::KeyRelease)
    {
        QKeyEvent* key = static_cast<QKeyEvent*>(event);
        if (key->key() == Qt::Key_Up || key->key() == Qt::Key_Down)
        {
            emit selectedChanged(tree_->currentIndex());
        }
    }
    return QObject::eventFilter(obj, event);
}
