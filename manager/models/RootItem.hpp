#pragma once

#include <QObject>

#include "ProjectModelItem.hpp"


class QAbstractItemModel;

namespace QtNodes
{
class NodesModel;
class ModelAdapter;
class FlowScene;
} // namespace QtNodes

namespace dscontrol
{
/*!
\brief ProjectModel item that is the root item (i.e. unique item, has no
 parent)
 */
class RootItem final : public ProjectModelItem
{
public:
    RootItem(ProjectModel *model, const QString &caption);
    ~RootItem();

    Type type() const override;

    /*!
    \brief always return nullptr because it have not any parent
     */
    ProjectModelItem *parentItem() const override;
    int rowOfItem(ProjectModelItem *childItem) const override;
    ProjectModelItem *itemAtRow(int row) const override;

    const QString &caption() const override;
    void setCaption(const QString &caption) override;

    int size() const override;

    void dataChanged(int row, ProjectModelItem *child) override;
    void rowInserted(int row, ProjectModelItem *parent) override;
    void rowRemoved(int row, ProjectModelItem *parent) override;

protected:
    /*!
    \brief set ownershiep of child
    \warning child have to be allocated in dynamic memory
     */
    bool addChild(ProjectModelItem *child) override;

private:
    ProjectModel *model_;
    ProjectModelItem *root_;

    std::vector<ProjectModelItem *> children_;

    QString caption_;
};
} // namespace dscontrol
