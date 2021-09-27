#pragma once

#include <QModelIndex>
#include <memory>
#include <vector>

class QStringList;
class QVariant;

class AbstractNodeItemMonitor
{
public:
    explicit AbstractNodeItemMonitor(AbstractNodeItemMonitor* parent = nullptr);
    virtual ~AbstractNodeItemMonitor() = default;

public:
    bool appendItemDataRole(int index, const Qt::ItemDataRole &itemRole);
    bool findItemDataRole(int index, const Qt::ItemDataRole &itemRole);
    bool readItemDataRole(int index, const Qt::ItemDataRole &readItemRole);
    bool removeItemDataRole(int index);
    bool isItemDataRole(int index, int isItemRole) const;

    const QModelIndex &modelIndex() const;
    AbstractNodeItemMonitor* childRow(int row);

    int rowCount() const;

    int childNumber() const;

    AbstractNodeItemMonitor* parent() const;
    bool insert(const std::pair<QString, AbstractNodeItemMonitor*> &item);
    void setModelIndex(const QModelIndex &index);
    bool removeAll(std::vector<std::pair<QString, AbstractNodeItemMonitor*>> &parent);
    bool remove(const QModelIndex &index);
    int getCheckState() const;
    virtual QVariant data(const QModelIndex &index, int role);
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    const QStringList &getValue() const;
    void setCheckState(int checkState);
    const std::vector<std::pair<QString, AbstractNodeItemMonitor*>> &childs() const;
    void parent(AbstractNodeItemMonitor* parent);

protected:
    int checkState_;
    QStringList value_;
    std::vector<std::pair<QString, AbstractNodeItemMonitor*>> childs_;
    AbstractNodeItemMonitor* parent_;
    QModelIndex modelIndex_;
    std::map<int, Qt::ItemDataRole> itemRoles_;
};