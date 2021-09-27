#include "ProjectTreeItem.hpp"

#include "singletones/Facade.hpp"

namespace dscontrol
{
ProjectTreeItem::ProjectTreeItem(const QVector<QVariant> &data, const QUuid &id, ProjectModel::MainProjectItems type,
                                 ProjectTreeItem* parent)
: type_ {type}
, id_ {id}
, itemData_(data)
, parentItem_ {parent}
{
}

ProjectTreeItem::~ProjectTreeItem()
{
    qDeleteAll(childItems_);
}

ProjectModel::MainProjectItems ProjectTreeItem::type()
{
    return type_;
}

QUuid ProjectTreeItem::id() const
{
    return id_;
}

void ProjectTreeItem::appendChild(ProjectTreeItem* item)
{
    childItems_.append(item);
}

ProjectTreeItem* ProjectTreeItem::child(int row)
{
    bool isRowValid = row >= 0 && row < childItems_.size();
    return isRowValid ? childItems_.at(row) : nullptr;
}

int ProjectTreeItem::childCount() const
{
    return childItems_.count();
}

int ProjectTreeItem::columnCount() const
{
    return itemData_.count();
}

QVariant ProjectTreeItem::data(int column) const
{
    if (column < 0 || column >= itemData_.size())
        return QVariant();
    return itemData_.at(column);
}

ProjectTreeItem* ProjectTreeItem::parentItem()
{
    return parentItem_;
}

int ProjectTreeItem::row() const
{
    if (parentItem_)
        return parentItem_->childItems_.indexOf(const_cast<ProjectTreeItem*>(this));

    return 0;
}

QString ProjectTreeItem::mimeCaption()
{
    switch (type_)
    {
        case ProjectModel::MainProjectItems::NestedItem:
        {
            auto model = QtNodes::Facade::Instance().model();
            switch (model->nodeType(model->nodeIndex(id_)))
            {
                case QtNodes::Item::NodeType::SystemNode:
                    return "System";
                case QtNodes::Item::NodeType::SubSystemNode:
                    return "Subsystem";
                case QtNodes::Item::NodeType::ComputerNode:
                    return "Device";
                case QtNodes::Item::NodeType::ApplicationNode:
                    return "Application";
                case QtNodes::Item::NodeType::ComponentNode:
                    return "Component";
                default:
                    return "";
            }
        }
        default:
            return "";
    }
}

void ProjectTreeItem::setCaption(QString caption)
{
    itemData_ = {caption};
}

} // namespace dscontrol
