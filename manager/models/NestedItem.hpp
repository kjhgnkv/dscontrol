#pragma once

#include <QColor>
#include <QObject>
#include <QUuid>
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
class ProjectModel;

/*!
\brief Represents a class that is a child item in project model
 */
class NestedItem final
    : public QObject
    , public ProjectModelItem
{
    Q_OBJECT
public:
    /*!
    \brief not set ownership of model
    \param parent set ownership of item
     */
    NestedItem(ProjectModelItem *parent, const QColor &backColor, const QString &caption = tr("Node1"), QtNodes::FlowScene *scene = nullptr);
    ~NestedItem() override;

    Type type() const override;

    ProjectModelItem *parentItem() const override;
    const QString &caption() const override;

    const QColor &backColor() const;

private:
    QString caption_;

    ProjectModelItem *parent_;

    QtNodes::ModelAdapter *nestedModel_;

    QtNodes::FlowScene *scene_;

    QColor backColor_;

    QUuid id_;

    QtNodes::Item::DeploymentType deplType_;

    // ProjectModelItem interface
public:
    void setCaption(const QString &caption);
    void setBackColor(const QColor &color);

    // ProjectModelItem interface
public:
    QtNodes::FlowScene *flowScene() const;
    QString mimeCaption();

    // ProjectModelItem interface
public:
    const QUuid id() const override;

    // ProjectModelItem interface
public:
    void setId(const QUuid &id) override;

    // ProjectModelItem interface
public:
    QtNodes::Item::DeploymentType deploymentType() const override;
    void setDeploymentType(const QtNodes::Item::DeploymentType &id) override;
};
} // namespace dscontrol
