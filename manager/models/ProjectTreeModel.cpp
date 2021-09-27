#include "ProjectTreeModel.hpp"

#include "nodeeditor/nodes/NodeIndex.hpp"
#include "nodeeditor/graphicsComposites/universalGraphicsObject/UniversalGraphicsItem.h"
#include "nodeeditor/models/NodesScene.hpp"
#include "nodeeditor/nodes/FlowItemMimeData.hpp"
#include "nodeeditor/nodes/UniversalGraphicsItemImpl.h"
#include "nodeeditor/singletones/Facade.hpp"

#include "ProjectTreeItem.hpp"

#include <QDebug>
#include <QStringList>

using namespace dscontrol;
using namespace QtNodes;

ProjectTreeModel::ProjectTreeModel(QObject* parent)
: QAbstractItemModel(parent)
, rootItem_ {nullptr}
, commItem_ {nullptr}
, messItem_ {nullptr}
{
    nodesModel_ = Facade::Instance().model();
    sceneModel_ = Facade::Instance().sceneModel();
    update();
}

Qt::ItemFlags ProjectTreeModel::flags(const QModelIndex &index) const
{
    if (index.isValid())
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled;
    }
    else
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
}

QStringList ProjectTreeModel::mimeTypes() const
{
    return {FlowItemMimeData::mimeType()};
}

QMimeData* ProjectTreeModel::mimeData(const QModelIndexList &indexes) const
{
    qDebug() << "generate subsystem mime";

    if (indexes.size() == Columns::Size)
    {
        if (const auto &curIndex = *indexes.begin(); checkIndex(curIndex))
        {
            auto item = static_cast<ProjectTreeItem*>(curIndex.internalPointer());

            QString sName = item->mimeCaption();
            QJsonObject name;
            name["caption"] = sName;
            QJsonObject node;
            node["implementation"] = name;
            node["id"] = item->id().toString();

            return new FlowItemMimeData {sName + "Node", node};
        }
    }

    return nullptr;
}

void ProjectTreeModel::setProject(const QString &projectName)
{
    setProjectName(projectName);
    update();
}

void ProjectTreeModel::setProjectName(const QString &projectName)
{
    projectName_ = projectName;
    if (nullptr != rootItem_)
    {
        rootItem_->setCaption(projectName_);
    }
}

ProjectTreeModel::~ProjectTreeModel()
{
    if (nullptr != rootItem_)
    {
        delete rootItem_;
        rootItem_ = nullptr;
    }
}

int ProjectTreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<ProjectTreeItem*>(parent.internalPointer())->columnCount();
    return rootItem_->columnCount();
}

QVariant ProjectTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant {};
    }

    ProjectTreeItem* item = static_cast<ProjectTreeItem*>(index.internalPointer());
    if (nullptr == item)
    {
        return QVariant {};
    }
    switch (role)
    {
        case Qt::DisplayRole:
        {
            return item->data(index.column());
        }
        case Qt::DecorationRole:
        {
            if (!nodesModel_)
            {
                return QPixmap {};
            }
            const auto* projectItem = getProjectItem(index);
            if (nullptr == projectItem)
            {
                return QPixmap {};
            }

            QPixmap pix;
            auto type = nodesModel_->nodeDeploymentType(nodesModel_->nodeIndex(projectItem->id()));
            switch (type)
            {
                case Item::DeploymentType::Internal:
                {
                    pix = QPixmap {":/structureIcons/resources/icons/I.svg"};
                    break;
                }
                case Item::DeploymentType::External:
                {
                    pix = QPixmap {":/structureIcons/resources/icons/E.svg"};
                    break;
                }
                default:
                {
                    break;
                }
            }
            return pix.isNull() ? pix : pix.scaled(QSize {12, 12}, Qt::IgnoreAspectRatio, Qt::FastTransformation);
        }
        case ProjectModel::Roles::Uid:
        {
            return item->id();
        }
        case ProjectModel::Roles::ProjectItem:
        {
            return QVariant::fromValue(static_cast<int>(item->type()));
        }
        case ProjectModel::Roles::FlowScene:
        {
            return QVariant::fromValue(Facade::Instance().sceneModel()->getScene(item->id()).get());
        }
        default:
        {
            return QVariant {};
        }
    }
}

/*Qt::ItemFlags ProjectTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}*/

QVariant ProjectTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem_->data(section);

    return QVariant();
}

QModelIndex ProjectTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    ProjectTreeItem* parentItem;

    if (!parent.isValid())
        parentItem = rootItem_;
    else
        parentItem = static_cast<ProjectTreeItem*>(parent.internalPointer());

    ProjectTreeItem* childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex ProjectTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    ProjectTreeItem* childItem = static_cast<ProjectTreeItem*>(index.internalPointer());
    ProjectTreeItem* parentItem = childItem->parentItem();

    if (parentItem == rootItem_)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int ProjectTreeModel::rowCount(const QModelIndex &parent) const
{
    if (true)
    {
        ProjectTreeItem* parentItem;
        if (parent.column() > 0)
            return 0;

        if (!parent.isValid())
            parentItem = rootItem_;
        else
            parentItem = static_cast<ProjectTreeItem*>(parent.internalPointer());

        return parentItem->childCount();
    }
    else
        return 0;
}

void ProjectTreeModel::update()
{
    if (nodesModel_ && sceneModel_)
    {
        beginResetModel();

        if (rootItem_)
        {
            delete rootItem_;
            rootItem_ = nullptr;
        }
        rootItem_ = new ProjectTreeItem({projectName_}, QUuid {}, ProjectModel::MainProjectItems::Root);
        setupModelData(rootItem_);

        endResetModel();
    }
}

void ProjectTreeModel::addChilds(ProjectTreeItem* parent)
{
    for (auto node : nodesModel_->nodeChilds(nodesModel_->nodeIndex(parent->id())))
    {
        auto item = new ProjectTreeItem({nodesModel_->nodeCaption(node)}, node.id(),
                                        ProjectModel::MainProjectItems::NestedItem, parent);
        parent->appendChild(item);
        addChilds(item);
    }
}

void ProjectTreeModel::setupModelData(ProjectTreeItem* root)
{
    if (!projectName_.isEmpty())
    {
        for (auto node : nodesModel_->nodes(Item::NodeType::SystemNode))
        {
            auto item = new ProjectTreeItem({nodesModel_->nodeCaption(node)}, node.id(),
                                            ProjectModel::MainProjectItems::NestedItem, root);
            root->appendChild(item);
            addChilds(item);
        }
        for (auto node : nodesModel_->nodes(Item::NodeType::SubSystemNode))
        {
            if (!nodesModel_->nodeParent(node).isValid())
            {
                auto item = new ProjectTreeItem({nodesModel_->nodeCaption(node)}, node.id(),
                                                ProjectModel::MainProjectItems::NestedItem, root);
                root->appendChild(item);
                addChilds(item);
            }
        }
        for (auto node : nodesModel_->nodes(Item::NodeType::ComputerNode))
        {
            if (!nodesModel_->nodeParent(node).isValid())
            {
                auto item = new ProjectTreeItem({nodesModel_->nodeCaption(node)}, node.id(),
                                                ProjectModel::MainProjectItems::NestedItem, root);
                root->appendChild(item);
                addChilds(item);
            }
        }
        for (auto node : nodesModel_->nodes(Item::NodeType::ApplicationNode))
        {
            if (!nodesModel_->nodeParent(node).isValid())
            {
                auto item = new ProjectTreeItem({nodesModel_->nodeCaption(node)}, node.id(),
                                                ProjectModel::MainProjectItems::NestedItem, root);
                root->appendChild(item);
                addChilds(item);
            }
        }

        commItem_ = new ProjectTreeItem({tr("Communications")}, QUuid {},
                                        ProjectModel::MainProjectItems::Communications, root);
        root->appendChild(commItem_);

        messItem_ = new ProjectTreeItem({tr("Messages")}, QUuid {}, ProjectModel::MainProjectItems::Messages, root);
        root->appendChild(messItem_);
    }
}

ProjectTreeItem* ProjectTreeModel::getProjectItem(const QModelIndex &index) const
{
    if (index.isValid())
    {
        return reinterpret_cast<ProjectTreeItem*>(index.internalPointer());
    }
    return nullptr;
}

void ProjectTreeModel::translateCaptions()
{
    if (commItem_)
    {
        commItem_->setCaption(tr("Communications"));
    }
    if (messItem_)
    {
        messItem_->setCaption(tr("Messages"));
    }
}
