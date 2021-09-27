#pragma once

#include <QAbstractItemModel>
#include <vector>

namespace dsrepository
{
class Component;
}

namespace dscontrol
{
class StoredItem;

/*!
\brief model store mime datas for nodeeditor. Have 2 levels: first level -
  category of component, second - config information for mime data
 */
class MimeStoreModel final : public QAbstractItemModel
{
public:
    enum Columns
    {
        Caption,
        Size
    };
    enum ModelRoles
    {
        JsonRole = Qt::UserRole + 1,
        TypeRole,
        UniqiueRole,
    };

    using QAbstractItemModel::QAbstractItemModel;
    MimeStoreModel(QObject *parent = nullptr);
    ~MimeStoreModel() override;

    int rowCount(const QModelIndex &parent = {}) const override;

    int columnCount(const QModelIndex &parent = {}) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = {}) const override;

    QModelIndex parent(const QModelIndex &index) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QStringList mimeTypes() const override;

    /*!
    \warning only if indexes.size() == 1, othrewise return nullptr
     */
    QMimeData *mimeData(const QModelIndexList &indexes) const override;

    /*!
    \brief use this method for load config information for create component
    \return true if added was be successfull, false - otherwise
    \param type type of Node (in FlowModel)
    \param category first level of model, in which will be added config
     information
    \param obj config information about component
     */
    bool addComponentConfig(const dsrepository::Component &component);
    void removeComponentConfig(const dsrepository::Component &component);

    int indexType(QModelIndex index);
    const QJsonObject storedItemData(QModelIndex index);
    StoredItem *storedItem(const QModelIndex &index);

    void clear();

    void insertRow(const int &row, const QModelIndex &parent);
    void removeRow(const int &row, const QModelIndex &parent);

protected:
    bool insertRows(int row, int count, const QModelIndex &parent = {}) override;
    bool removeRows(int row, int count, const QModelIndex &parent = {}) override;

private:
    // here we store values for create FlowItemMimeData
    std::vector<StoredItem *> categories_;
    StoredItem *root_;
};

} // namespace dscontrol
