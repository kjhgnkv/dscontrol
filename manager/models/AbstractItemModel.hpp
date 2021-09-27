#pragma once

#include <QAbstractItemModel>
#include <memory>

class AbstractNodeItem;

class AbstractItemModel : public QAbstractItemModel
{
public:
    explicit AbstractItemModel(AbstractNodeItem* root = nullptr);
    ~AbstractItemModel() override;

    QModelIndex insert(const QVariant &variant, const QModelIndex &parent = QModelIndex());
    QModelIndex insert(AbstractNodeItem* nodeItem, const QModelIndex &parent = QModelIndex());
    bool update(const QModelIndex &index, const QVariant &variant, int role = Qt::EditRole);
    bool remove(const QModelIndex &parent);

    AbstractNodeItem* child(const QModelIndex &index) const;
    AbstractNodeItem* root();

    void appendTitle(const QStringList &listTitle);
    void appendTitle(const QString &title);

    void editTitle(const QString &title, int index, const Qt::Orientation &orientation = Qt::Horizontal);
    void removeTitle();
    void removeTitle(int position);

    void setQtFlag(Qt::ItemFlag flag);
    void setQtFlags(Qt::ItemFlags flags);
    void appendQtFlag(Qt::ItemFlag flag);

    bool updateData(const QModelIndex &index);

    bool removeAll(const QModelIndex &modelIndex = QModelIndex());

    void allUnchecked();

protected:
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role) override;

    bool insertRow(int row, const QModelIndex &parent = QModelIndex());
    bool insertColumn(int column, const QModelIndex &parent = QModelIndex());
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    bool removeColumn(int column, const QModelIndex &parent = QModelIndex());

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override;
    bool moveColumns(const QModelIndex &sourceParent, int sourceColumn, int count, const QModelIndex &destinationParent,
                     int destinationChild) override;

    bool insertRows(AbstractNodeItem* nodeItem, int position, int rows, const QModelIndex &parent);
    bool insertRow(AbstractNodeItem* nodeItem, int arow, const QModelIndex &aparent);

    bool setCheckState(AbstractNodeItem* parent, Qt::CheckState state);

protected:
    AbstractNodeItem* rootItem_;
    QStringList stringTitle_;
    Qt::ItemFlags itemFlags_;
};