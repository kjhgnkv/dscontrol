#include "RootItem.hpp"

#include "ProjectModel.hpp"

using namespace dscontrol;

RootItem::RootItem(ProjectModel* item, const QString &caption)
: model_ {item}
, caption_ {caption}
{
}

RootItem::~RootItem()
{
    for (auto* i : children_)
    {
        delete i;
    }
}

ProjectModelItem::Type RootItem::type() const
{
    return Type::Root;
}

ProjectModelItem* RootItem::parentItem() const
{
    return nullptr;
}

int RootItem::rowOfItem(ProjectModelItem* childItem) const
{
    const auto &found = std::find_if(children_.begin(), children_.end(), [childItem](const auto &val)
    {
        return val == childItem;
    });

    const int errorRetValue {-1};
    return found != children_.end() ? std::distance(children_.begin(), found) : errorRetValue;
}

ProjectModelItem* RootItem::itemAtRow(int row) const
{
    const bool isRowValid = row >= 0 && row < size();
    return isRowValid ? children_[row] : nullptr;
}

const QString &RootItem::caption() const
{
    return caption_;
}

void RootItem::setCaption(const QString &caption)
{
    caption_ = caption;
}

int RootItem::size() const
{
    return children_.size();
}

bool RootItem::addChild(ProjectModelItem* child)
{
    if (child)
    {
        children_.push_back(child);
        return true;
    }
    return false;
}

void RootItem::dataChanged(int row, ProjectModelItem* updated)
{
    if (!model_)
    {
        return;
    }
    auto index = model_->getIndexFromProjectItem(row, updated);
    if (index.isValid())
    {
        model_->dataChanged(index, index);
    }
}

void RootItem::rowInserted(int row, ProjectModelItem* parent)
{
    // here row - row which have to be inserted for parent
    if (!model_)
    {
        return;
    }
    auto parentIndex = model_->getIndexFromProjectItem(rowOfItem(parent), parent);
    if (parentIndex.isValid())
    {
        model_->insertRow(row, parentIndex);
    }
}

void RootItem::rowRemoved(int row, ProjectModelItem* parent)
{
    if (!model_)
    {
        return;
    }
    auto parentIndex = model_->getIndexFromProjectItem(rowOfItem(parent), parent);
    if (parentIndex.isValid())
    {
        model_->removeRow(row, parentIndex);
    }
}
