#pragma once

#include "models/AbstractNodeItem.hpp"

struct UpdateComponentLevel
{
    enum
    {
        Class,
        Component
    };
};

class NodeItemUpdateComponent : public AbstractNodeItem
{
public:
    explicit NodeItemUpdateComponent();
    explicit NodeItemUpdateComponent(const QString &caption, const QString &type, int level);
    QVariant data(const QModelIndex &index, int role) override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    int getLevel() const;

private:
    int level_;
};