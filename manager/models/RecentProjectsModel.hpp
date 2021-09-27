#pragma once

#include <QStringListModel>


namespace dscontrol
{
/*!
\brief Model class for recent projects list
 */
class RecentProjectsModel final : public QStringListModel
{
public:
    enum Role
    {
        FullFileName = Qt::UserRole + 1
    };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    /*!
    \brief does nothing - model can not be changed by this
     */
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
};
} // namespace dscontrol
