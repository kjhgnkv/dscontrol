#pragma once

#include <QModelIndex>
#include <memory>

class AbstractNodeItem
{
public:
    bool appendItemDataRole(int index, const Qt::ItemDataRole &itemRole);
    bool findItemDataRole(int index, const Qt::ItemDataRole &itemRole);
    bool readItemDataRole(int index, const Qt::ItemDataRole &readItemRole);
    bool removeItemDataRole(int index);
    bool isItemDataRole(int index, int isItemRole) const;

public:
    explicit AbstractNodeItem(AbstractNodeItem* parent = nullptr);
    virtual ~AbstractNodeItem() = default;

public:
    const QModelIndex &getModelIndex() const;
    AbstractNodeItem* childRow(int row);

    int rowCount() const;

    int childNumber() const;

    AbstractNodeItem* parent() const;
    bool insert();
    bool insert(AbstractNodeItem* item);
    void setModelIndex(const QModelIndex &index);
    bool removeAll(std::vector<AbstractNodeItem*> &parent);
    bool remove(const QModelIndex &index);
    const std::vector<AbstractNodeItem*> &childs() const;
    int getCheckState() const;
    virtual QVariant data(const QModelIndex &index, int role);
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    const QStringList &getValue() const;
    void setCheckState(int checkState);

protected:
    void parent(AbstractNodeItem* parent);

protected:
    int checkState_;
    QStringList value_;
    std::vector<AbstractNodeItem*> childs_;
    AbstractNodeItem* parent_;
    QModelIndex modelIndex_;
    std::map<int, Qt::ItemDataRole> itemRoles_;
};