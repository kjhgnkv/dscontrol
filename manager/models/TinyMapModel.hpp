#pragma once

#include <QStringListModel>


namespace dscontrol
{
/*!
\brief Model class for available communications table in ConnectionCreatorDialog
and MsgCommDialog
 */
class TinyMapModel : public QAbstractListModel
{
    Q_OBJECT
private:
    std::map<QString, Qt::CheckState> allCommNames_;
    std::map<QString, Qt::CheckState>::const_iterator lastCheck_;
    QStringList createdCommNames_;

public:
    explicit TinyMapModel(const QStringList &lastCommNames, const QStringList &allCommNames, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    /*!
    \brief Set checked communications
    */
    void setLastCommNames(const QStringList &lastCommNames);

    /*!
    \return All checked communication names
    */
    QStringList getLastCommNames() const;

    /*!
    \brief Set checked communications names that are already created
    */
    void setCreatedCommNames(const QStringList &createdCommNames);

    /*!
    \return Created communications
    */
    QStringList getCreatedCommNames() const;

    /*!
    \brief Sets all communications (unchecked)
    */
    void setAllCommNames(const QStringList &allCommNames);
    ~TinyMapModel() override = default;
};
} // namespace dscontrol
