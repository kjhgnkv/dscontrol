#include "DeployTargetItem.hpp"

using namespace dscontrol::models;

DeployTargetItem::DeployTargetItem(const QString &name, const QString &ip
    , bool ipVisible, DeployTargetItem* parent)
: name_ {name}
, ip_ {ip}
, ipVisible_ {ipVisible}
, parent_ {parent}
, children_ {}
, checkState_ {Qt::CheckState::Unchecked}
{
    if (parent_)
    {
        parent_->addChildren(this);
    }
}

DeployTargetItem::~DeployTargetItem()
{
    for (const auto &child : children_)
    {
        delete child;
    }

    children_.clear();
}

void DeployTargetItem::clear()
{
    for (const auto &child : children_)
    {
        delete child;
    }

    children_.clear();
}

void DeployTargetItem::addChildren(DeployTargetItem* child)
{
    children_.append(child);
}

void DeployTargetItem::setIp(const QString &ip)
{
    ip_ = ip;
}

void DeployTargetItem::setIpVisible(bool ipVisible)
{
    ipVisible_ = ipVisible;
}

void DeployTargetItem::setChecked(Qt::CheckState check)
{
    checkState_ = check;
}

QString DeployTargetItem::name() const
{
    return name_;
}

QString DeployTargetItem::ip() const
{
    return ip_;
}

bool DeployTargetItem::isIpVisible() const
{
    return ipVisible_;
}

Qt::CheckState DeployTargetItem::isChecked() const
{
    return checkState_;
}

DeployTargetItem* DeployTargetItem::parent() const
{
    return parent_;
}

QList<DeployTargetItem*> DeployTargetItem::children() const
{
    return children_;
}

DeployTargetItem* DeployTargetItem::child(int row) const
{
    if (row >= 0 && row < children_.size())
    {
        return children_[row];
    }

    return nullptr;
}

int DeployTargetItem::childIndex(DeployTargetItem* child) const
{
    return children_.indexOf(child);
}
