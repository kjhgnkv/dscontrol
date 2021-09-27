#pragma once

#include <QTreeView>

namespace dscontrol
{
namespace CustomEventFilters
{

/*!
\brief Event Filter class for project by layers and project by nesting
 widgets
\param tree: tree view need to be configured

Handles: Del key press (delete selected node), Up\Down key pressed (select node
on scene)
 */
class ProjectTreeEventFilter : public QObject
{
Q_OBJECT
public:
    ProjectTreeEventFilter(QTreeView* tree, QObject* parent = nullptr);

protected:
    bool eventFilter(QObject* obj, QEvent* event);

signals:
    void delPressed(const QUuid &);
    void selectedChanged(const QModelIndex &);

private:
    QTreeView* tree_;
};

} // namespace CustomEventFilters
} // namespace dscontrol
