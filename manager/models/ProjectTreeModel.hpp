#pragma once

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QUuid>
#include <QVariant>


namespace QtNodes
{
class DataFlowModel;
class NodesScene;
} // namespace QtNodes

namespace dscontrol
{
class ProjectTreeItem;

/*!
\brief Model class for project by nesting widgets tree view
 */
class ProjectTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum Columns
    {
        Caption,
        Size
    };

    explicit ProjectTreeModel(QObject *parent = nullptr);
    ~ProjectTreeModel();

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QStringList mimeTypes() const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;

    /*!
    \brief Resets and rebuilds all the model by calling setupModelData
     */
    void update();

    /*!
    \brief Setup the project name which is represents as a tree view header
    \param projectName Name of the project, if empty model will be empty too;
     use this options to hide project tree when no project is opened
     */
    void setProject(const QString &projectName);

    void setProjectName(const QString &projectName);

    /*!
    \return Item(node) for target model index in tree view
     */
    ProjectTreeItem *getProjectItem(const QModelIndex &index) const;

    /*!
    \brief update captions in dependency on language
     */
    void translateCaptions();

private:
    QString projectName_;

    QUuid uid_;

    /*!
    \brief Setup the model. Gets information about all levels (System,
     Subsystem, Computers, Applications) and add it in tree-like model
    \param parent root item
     */
    void setupModelData(ProjectTreeItem *parent);

    /*
    \brief Adds all childs to target parent recursively
    \warning recursive
     */
    void addChilds(ProjectTreeItem *parent);

    ProjectTreeItem *rootItem_;
    ProjectTreeItem *commItem_;
    ProjectTreeItem *messItem_;

    QtNodes::DataFlowModel *nodesModel_;
    QtNodes::NodesScene *sceneModel_;
};
} // namespace dscontrol
