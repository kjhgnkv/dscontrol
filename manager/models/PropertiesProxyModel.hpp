#pragma once

#include <QSortFilterProxyModel>

namespace dscontrol
{
class PropertiesProxyModel final : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit PropertiesProxyModel(QObject *parent = nullptr);
    ~PropertiesProxyModel() override = default;

    void setFilterStr(const QString &filter);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    bool containsStr(const QModelIndex &sourceIndex) const;

private:
    QString filterStr_;
};
} // namespace dscontrol
