#pragma once

#include <QAbstractItemModel>

#include "nodeeditor/models/ModelAdapter.hpp"
#include "nodeeditor/scenes/FlowScene.hpp"

namespace dscontrol
{
class ProjectModelItem;

class Project;

/*!
\brief tree model which represent Project. Model have several levels:
  1) root - root element, name of current project. We can not remove or change
  this element
  2) System, Computers and vice versa. We can not add or remove
  items on this level
  3) here we represent dynamic elements. Here we can add and remove elements
  on this level
 */
class ProjectModel final : public QAbstractItemModel
{
    Q_OBJECT
public:
    /*!
    \brief Types of model items
    */
    enum MainProjectItems
    {
        Invalid,
        Root,
        Systems,
        SubSystems,
        Computers,
        Applications,
        Components,
        Communications,
        Messages,
        NestedItem,
    };

    enum Columns
    {
        Caption,
        Size
    };
    enum Roles
    {
        FlowScene = Qt::UserRole + 1,
        ProjectItem,
        Uid
    };

    explicit ProjectModel(QObject *parent = nullptr);

    ~ProjectModel() override;

    void updateProjectName();

    int rowCount(const QModelIndex &index = {}) const override;
    int columnCount(const QModelIndex &index = {}) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    QModelIndex parent(const QModelIndex &index) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = {}) const override;

    /*!
    \brief set current project to model.
    \param project if equal nullptr - then clean just clean model
    \warning not set ownership of project
     */
    void setProject(Project *project);

    bool insertRows(int row, int count, const QModelIndex &parent) override;

    bool removeRows(int row, int count, const QModelIndex &parent) override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QStringList mimeTypes() const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;

    /*!
    \return nullptr if index is not valid or if was error
     */
    ProjectModelItem *getProjectItem(const QModelIndex &index) const;

    /*!
    \return Model index  created from target item, invalid if invalid row or
     item
     */
    QModelIndex getIndexFromProjectItem(int row, ProjectModelItem *item) const;

    /*!
    \return Model index, that is the root index
     */
    QModelIndex getRootIndex();

    /*!
    \brief update captions in dependency on language
     */
    void translateCaptions();

private:
    Project *project_;

    ProjectModelItem *root_;
    ProjectModelItem *subSystemRoot_;
    ProjectModelItem *componentRoot_;
    ProjectModelItem *computerRoot_;
    ProjectModelItem *applicationRoot_;
    ProjectModelItem *systemRoot_;
    ProjectModelItem *communicationRoot_;
    ProjectModelItem *messagesRoot_;

    QtNodes::ModelAdapter *adapter_;
};
} // namespace dscontrol

// we declare it here, because index can return this value
Q_DECLARE_METATYPE(QtNodes::FlowScene *)
Q_DECLARE_METATYPE(dscontrol::ProjectModel::MainProjectItems)
