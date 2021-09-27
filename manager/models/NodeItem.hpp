#pragma once

#include "ProjectModelItem.hpp"
#include "nodeeditor/common/nodeeditor_globals.hpp"


class QAbstractItemModel;

namespace QtNodes
{
class NodesModel;
class ModelAdapter;
class FlowScene;
} // namespace QtNodes

namespace dscontrol
{
class ProjectModel;

/*!
\brief Represents an item that is the main item in Project model (i.e.
 systems, subsystems, messages etc.)
 */
class NodeItem final
    : public QObject
    , public ProjectModelItem
{
    Q_OBJECT
public:
    /*!
    \brief not set ownership of model
    \param parent Set ownership of item
    \param adapter Model of nodes type. Should be nullptr if communications
     or messages
    \param caption Caption of item
    \param type Type of item
     */
    NodeItem(ProjectModelItem *parent, QtNodes::ModelAdapter *adapter, const QString &caption, const Type &type);
    ~NodeItem() override;

    /*!
    \return Type of item
     */
    Type type() const override;

    /*!
    \brief setup nodes data model
     */
    void setFlowModel(QtNodes::DataFlowModel *model) override;

    /*!
    \return parent item
     */
    ProjectModelItem *parentItem() const override;

    /*!
    \return row of  target item. return -1 if has no such item.
     */
    int rowOfItem(ProjectModelItem *childItem) const override;

    /*!
    \return item at target row. nullptr if has no such row
     */
    ProjectModelItem *itemAtRow(int row) const override;

    /*!
    \return caption of this item
     */
    const QString &caption() const override;

    /*!
    \return children count
     */
    int size() const override;

    /*!
    \brief update children.
     */
    void dataChanged(int row, ProjectModelItem *item) override;
    void rowInserted(int row, ProjectModelItem *parent) override;
    void rowRemoved(int row, ProjectModelItem *parent) override;

    void removeItemAtRow(int row) override;

protected slots:
    void itemAdded(const QModelIndex &parent, int first, int last);
    /*!
    \brief call before removing item from nested model
     */
    void itemRemoved(const QModelIndex &index, int first, int last);
    void itemChanged(const QModelIndex &index);

private:
    QString caption_;

    ProjectModelItem *parent_;
    Type type_;

    QtNodes::ModelAdapter *nestedModel_;

    /*!
    \brief construct from nestedModel indexes
     */
    std::vector<ProjectModelItem *> children_;

    // ProjectModelItem interface
public:
    /*!
    \brief Set new caption
    \param new caption
     */
    void setCaption(const QString &caption) override;
};

} // namespace dscontrol
