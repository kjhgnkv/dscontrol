#pragma once

#include <QSortFilterProxyModel>


namespace dscontrol
{
class AllComponentsModel;

/*!
\brief proxy model for dscontrol models
 */
class DSProxyModel final : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    /*!
    \brief model uses this filter with source indexes. If return true - then
    index will be displayed in view
     */
    using Filter = std::function<bool(const QModelIndex &sourceIndex)>;

    explicit DSProxyModel(QObject *parent = nullptr);
    ~DSProxyModel() override = default;

    /*!
    \brief set filter for dscontrol models
     */
    void setFilter(const Filter &filter, const int &column = 1);

private:
    Filter filter_;
    int column_;

    // QAbstractItemModel interface
public:
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
};
} // namespace dscontrol
