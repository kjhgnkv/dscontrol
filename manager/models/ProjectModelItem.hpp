#pragma once

#include <QObject>

#include "nodeeditor/common/nodeeditor_globals.hpp"


class QAbstractItemModel;

namespace QtNodes
{
class NodesModel;
class DataFlowModel;
class ModelAdapter;
class FlowScene;
} // namespace QtNodes

namespace dscontrol
{
class ProjectModel;

/*!
\brief Represents a class that is the base class for items in project model
 */
class ProjectModelItem
{
public:
    enum class Type
    {
        Root,
        System,
        SubSystem,
        Computers,
        Applications,
        Component,
        Communications,
        Messages,
        NestedItem
    };
    virtual ~ProjectModelItem() = default;

    virtual Type type() const = 0;

    virtual void setFlowModel(QtNodes::DataFlowModel *model);

    virtual ProjectModelItem *parentItem() const = 0;
    /*!
    \return valid row (0-...) if item in children, if not - -1. By default
     return -1
     */
    virtual int rowOfItem(ProjectModelItem *childItem) const;
    /*!
    \brief by default does nothing
     */
    virtual ProjectModelItem *itemAtRow(int row) const;

    virtual const QString &caption() const = 0;

    virtual void setCaption(const QString &caption);

    /*!
    \brief by default return 0
     */
    virtual int size() const { return 0; }

    virtual QString mimeCaption();

    /*!
    \brief by default does nothing
     */
    virtual bool addChild(ProjectModelItem *child);

    /*!
    \brief update children. By default does nothing
     */
    virtual void dataChanged(int row, ProjectModelItem *child);

    /*!
    \brief about to be removed. By default does nothing
     */
    virtual void rowRemoved(int row, ProjectModelItem *parent);

    /*!
    \brief about to be inserted. By default does nothing
     */
    virtual void rowInserted(int row, ProjectModelItem *parent);

    // TODO I don't like it, but do not know how do it in other way
    /*!
    \return FlowScene which represent current item. By default return
     nullptr
     */
    virtual QtNodes::FlowScene *flowScene() const;

    // uses for removing item in model method
    virtual void removeItemAtRow(int row);

    /*!
    \return id of this item
     */
    virtual const QUuid id() const;

    /*!
    \brief setup id of item
     */
    virtual void setId(const QUuid &id);

    /*!
    \return deployment type of item
     */
    virtual QtNodes::Item::DeploymentType deploymentType() const;

    /*!
    \brief setup deployment type of item
     */
    virtual void setDeploymentType(const QtNodes::Item::DeploymentType &id);
};

} // namespace dscontrol
