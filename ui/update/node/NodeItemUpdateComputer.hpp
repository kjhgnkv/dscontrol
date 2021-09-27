#pragma once

#include "models/AbstractNodeItem.hpp"

class AbstractItemModel;

struct UpdateComputerLevel
{
    enum
    {
        Computer,
        Application,
        Component
    };
};

class NodeItemUpdateComputer : public AbstractNodeItem
{
public:
    explicit NodeItemUpdateComputer();
    explicit NodeItemUpdateComputer(const QString &value, int level);
    virtual ~NodeItemUpdateComputer();

    QVariant data(const QModelIndex &index, int role) override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    int getLevel() const;
    AbstractItemModel* ItemModelComponent() const;

private:
    int level_;
    AbstractItemModel* itemModelComponent_;
    QModelIndex selectModelIndexXML_;

public:
    const QModelIndex &getSelectModelIndexXml() const;
    void setSelectModelIndexXml(const QModelIndex &selectModelIndexXml);
};
