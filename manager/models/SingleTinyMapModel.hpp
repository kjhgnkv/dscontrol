#pragma once

#include <QStringListModel>


namespace dscontrol
{
/*!
\brief Model class for available senders\receivers table in
ConnectionCreatorDialog
 */
class SingleTinyMapModel : public QAbstractListModel
{
    Q_OBJECT
private:
    bool lastChecked_;
    std::map<QString, Qt::CheckState> allCommNames_;
    std::map<QString, Qt::CheckState>::iterator last_;
    QStringList createdCommNames_;

public:
    explicit SingleTinyMapModel(const QString &lastCommName, const QStringList &allCommNames, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    /*!
    \brief Set the last checked component
    \warning there is only one checked at the time
    */
    void setLastCommNames(const QString &lastCommNames);

    /*!
    \brief Set checked components names
    */
    void setCreatedCommNames(const QStringList &createdCommNames);

    /*!
    \return All checked components names
    */
    QStringList getLastCommNames() const;

    /*!
    \brief Sets all components names (unchecked)
    */
    void setAllCommNames(const QStringList &allCommNames);
    ~SingleTinyMapModel() override = default;
};
} // namespace dscontrol
