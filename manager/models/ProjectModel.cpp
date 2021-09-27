// ProjectModel.cpp

#include "ProjectModel.hpp"

#include "manager/Project.hpp"
#include "manager/models/NestedItem.hpp"
#include "manager/models/NodeItem.hpp"
#include "manager/models/ProjectModelItem.hpp"
#include "manager/models/RootItem.hpp"

#include "nodes/FlowItemMimeData.hpp"
#include "singletones/Facade.hpp"

#include <QDebug>
#include <QUuid>

using namespace dscontrol;
using namespace QtNodes::Item;
using namespace QtNodes;

// represent rows of MainItems
ProjectModel::ProjectModel(QObject* parent)
: QAbstractItemModel {parent}
, project_ {nullptr}
, root_ {nullptr}
, subSystemRoot_ {nullptr}
, componentRoot_ {nullptr}
, computerRoot_ {nullptr}
, applicationRoot_ {nullptr}
, systemRoot_ {nullptr}
, communicationRoot_ {nullptr}
, messagesRoot_ {nullptr}
{
}

ProjectModel::~ProjectModel()
{
    if (nullptr != root_)
    {
        delete root_;
    }
}

void ProjectModel::updateProjectName()
{
    if (nullptr != project_ && nullptr != root_)
    {
        root_->setCaption(project_->name());
    }
}

void ProjectModel::setProject(Project* project)
{
    qDebug() << "ProjectModel: new Project set in ProjectModel " << project;
    beginResetModel();
    if (project_)
    {
        qDebug() << "ProjectModel: removed previous project";
        // remove all ProjectItems
        delete root_;
        root_ = nullptr;
        disconnect(this, nullptr, project_, nullptr);
        disconnect(this, nullptr, project_, nullptr);
    }
    project_ = project;
    if (project_)
    {
        root_ = new RootItem {this, project_->name()};

        systemRoot_ = new NodeItem {
        root_, new ModelAdapter {NodeType::SystemNode}, tr("Systems"), ProjectModelItem::Type::System
        };
        subSystemRoot_ = new NodeItem {
        root_, new ModelAdapter {NodeType::SubSystemNode}, tr("SubSystems"), ProjectModelItem::Type::SubSystem
        };
        computerRoot_ = new NodeItem {
        root_, new ModelAdapter {NodeType::ComputerNode}, tr("Computers"), ProjectModelItem::Type::Computers
        };
        applicationRoot_ = new NodeItem {
        root_, new ModelAdapter {NodeType::ApplicationNode}, tr("Applications"), ProjectModelItem::Type::Applications
        };
        componentRoot_ = new NodeItem {
        root_, new ModelAdapter {NodeType::ComponentNode}, tr("Components"), ProjectModelItem::Type::Component
        };
        communicationRoot_ = new NodeItem {
        root_, nullptr, tr("Communications"), ProjectModelItem::Type::Communications
        };
        messagesRoot_ = new NodeItem {root_, nullptr, tr("Messages"), ProjectModelItem::Type::Messages};

        systemRoot_->setFlowModel(Facade::Instance().model());
        subSystemRoot_->setFlowModel(Facade::Instance().model());
        computerRoot_->setFlowModel(Facade::Instance().model());
        applicationRoot_->setFlowModel(Facade::Instance().model());
        componentRoot_->setFlowModel(Facade::Instance().model());

        connect(project_, &Project::destroyed, this, [this]()
        {
            beginResetModel();
            project_ = nullptr;
            delete root_;
            root_ = nullptr;
            endResetModel();
        });
    }
    endResetModel();
}

int ProjectModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        if (auto item = getProjectItem(parent))
        {
            return item->size();
        }
        qWarning() << "ProjectModel: asked unexpected index (" << parent.row() << ' ' << parent.column() << ' ' << parent.internalPointer();
    }
    else
    {
        if (root_)
        {
            // model have only one root element, so, here we return always 1 if
            // project is valid, ro 0 if not valid
            return 1;
        }
    }
    return 0;
}

int ProjectModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return Columns::Size;
}

QVariant ProjectModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return {};
    }
    switch (role)
    {
        case Qt::DisplayRole:
        {
            switch (index.column())
            {
                case Columns::Caption:
                {
                    if (auto item = getProjectItem(index))
                    {
                        return item->caption();
                    }
                    break;
                }
                default:
                {
                    break;
                }
            };
            break;
        }

        case Qt::DecorationRole:
        {
            const auto* projectItem = getProjectItem(index);
            if (nullptr == projectItem || projectItem == root_ || projectItem->parentItem() == root_)
            {
                return QPixmap {};
            }

            QPixmap pix;
            switch (projectItem->deploymentType())
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
        case Qt::BackgroundRole:
        {
            if (auto item = dynamic_cast<class NestedItem*>(getProjectItem(index)); item)
            {
                return item->backColor();
            }
            break;
        }
        case Qt::TextAlignmentRole:
        {
            return Qt::AlignLeft;
        }
        case Roles::FlowScene:
        {
            const auto* projectItem = getProjectItem(index);
            QtNodes::FlowScene* ret = nullptr != projectItem ? projectItem->flowScene() : nullptr;
            return QVariant::fromValue(ret);
        }
        case Roles::ProjectItem:
        {
            const auto* projectItem = getProjectItem(index);
            if (nullptr == projectItem)
            {
                return QVariant::fromValue(static_cast<int>(MainProjectItems::Invalid));
            }
            if (projectItem == root_)
            {
                return QVariant::fromValue(static_cast<int>(MainProjectItems::Root));
            }
            if (projectItem->parentItem() != root_)
            {
                return QVariant::fromValue(static_cast<int>(MainProjectItems::NestedItem));
            }
            const int enumSystemsValue = static_cast<int>(MainProjectItems::Systems);
            const int enumDiff = enumSystemsValue - static_cast<int>(MainProjectItems::Invalid);
            const int enumValue {index.row() + enumDiff};
            if (enumValue < enumSystemsValue || enumValue > static_cast<int>(MainProjectItems::Messages))
            {
                return {};
            }
            return QVariant::fromValue(enumValue);
        }
        case Roles::Uid:
        {
            const auto* projectItem = getProjectItem(index);
            const auto &id = nullptr != projectItem ? projectItem->id() : QUuid {};
            return QVariant::fromValue(id);
        }
        default:
        {
            break;
        }
    }
    return {};
}

QModelIndex ProjectModel::parent(const QModelIndex &index) const
{
    if (index.isValid())
    {
        if (auto item = getProjectItem(index))
        {
            // in this case we have parent, other it is root item (root item
            // have paretn as nullptr)
            if (auto parentItem = item->parentItem())
            {
                // if parent is valid, then we have to find parent of parent for
                // create right index
                if (auto parentOfParentItem = parentItem->parentItem())
                {
                    // find position of parent in children of parentOfParent
                    // (this will be row)
                    int row = parentOfParentItem->rowOfItem(parentItem);
                    if (row >= 0)
                    {
                        return getIndexFromProjectItem(row, parentItem);
                    }
                    // if not item not founded in children
                    qCritical() << "ProjectModel: item not founded as child of parent";
                }
                // if parentOfParentItem equal nullptr, then it means what
                // parent is root item
                return this->index(0, Columns::Caption, {});
            }
        }
    }
    return {};
}

QModelIndex ProjectModel::index(int row, int column, const QModelIndex &parent) const
{
    if (root_ && row >= 0 && column >= 0 && row < rowCount(parent) && column < columnCount(parent))
    {
        if (!parent.isValid())
        {
            // index store in internallPointer pointer to ProjectItem
            return createIndex(0, Columns::Caption, root_);
        }
        else
        {
            if (auto parentItem = getProjectItem(parent))
            {
                // here we not check out of range because we check this in the
                // main if
                if (auto item = parentItem->itemAtRow(row))
                {
                    return createIndex(row, column, item);
                }
                qCritical() << "ProjectModel: not valid item in children";
            }
        }
    }
    qWarning() << "in ProjectModel asked unkhnown index (" << row << ' ' << column << ", parent (" << parent.row() << ' ' << parent.column() << ' ' << parent.internalPointer() << ')' << ')';
    return {};
}

QVariant ProjectModel::headerData(int, Qt::Orientation orientation, int role) const
{
    const bool captionHasToBeenDisplayed = root_ && orientation == Qt::Horizontal && role == Qt::DisplayRole;
    return captionHasToBeenDisplayed ? root_->caption() : QVariant {};
}

QModelIndex ProjectModel::getRootIndex()
{
    return createIndex(0, Columns::Caption, root_);
}

void ProjectModel::translateCaptions()
{
    if (systemRoot_)
        systemRoot_->setCaption(tr("Systems"));
    if (subSystemRoot_)
        subSystemRoot_->setCaption(tr("SubSystems"));
    if (computerRoot_)
        computerRoot_->setCaption(tr("Computers"));
    if (applicationRoot_)
        applicationRoot_->setCaption(tr("Applications"));
    if (componentRoot_)
        componentRoot_->setCaption(tr("Components"));
    if (communicationRoot_)
        communicationRoot_->setCaption(tr("Communications"));
    if (messagesRoot_)
        messagesRoot_->setCaption(tr("Messages"));
}

ProjectModelItem* ProjectModel::getProjectItem(const QModelIndex &index) const
{
    if (index.isValid())
    {
        return reinterpret_cast<ProjectModelItem*>(index.internalPointer());
    }
    return nullptr;
}

QModelIndex ProjectModel::getIndexFromProjectItem(int row, ProjectModelItem* item) const
{
    if (row >= 0 && item)
    {
        return createIndex(row, Columns::Caption, item);
    }
    return {};
}

QStringList ProjectModel::mimeTypes() const
{
    return {FlowItemMimeData::mimeType()};
}

QMimeData* ProjectModel::mimeData(const QModelIndexList &indexes) const
{
    qDebug() << "generate subsystem mime";

    if (indexes.size() == Columns::Size)
    {
        if (const auto &curIndex = *indexes.begin(); checkIndex(curIndex))
        {
            auto item = getProjectItem(curIndex);

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

bool ProjectModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid())
    {
        beginInsertRows(parent, row, row + count - 1);
        endInsertRows();
        return true;
    }
    return false;
}

bool ProjectModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid())
    {
        beginRemoveRows(parent, row, row + count - 1);
        auto parentOfRemoved = getProjectItem(parent);
        for (int i = count - 1; i >= 0; --i)
        {
            parentOfRemoved->removeItemAtRow(row + i);
        }
        endRemoveRows();
        return true;
    }
    return false;
}

Qt::ItemFlags ProjectModel::flags(const QModelIndex &index) const
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
