#pragma once

#include <QAbstractTableModel>

namespace QtNodes
{
class CommunicationList;
}

namespace dscontrol
{
/*!
\brief Model class for Communications dialog table view
 */
class CommunicationModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Columns
    {
        DeviceType,
        DeviceName,
        CommName,
        Id,
        Method,
        Component,
        Sender,
        Recv,
        Fragment,
        Size
    };
    /*!
    \param commList pointer to list of communications
     */
    explicit CommunicationModel(QtNodes::CommunicationList *commList, QObject *parent = nullptr);

    virtual ~CommunicationModel() override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

public slots:
    void insertRow(const int &id);
    void removeRow(const int &id);

signals:
    void updateError(const QString &error);

private:
    QtNodes::CommunicationList *commList_;
};
} // namespace dscontrol
