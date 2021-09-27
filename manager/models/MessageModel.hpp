#pragma once

#include <QAbstractTableModel>

namespace QtNodes
{
class MessageList;
}

namespace dscontrol
{
/*!
\brief Model containing messages for messages dialog table
 */
class MessageModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Columns
    {
        Name,
        Id,
        Protocol,
        CommNames,
        Size,
        CommCount
    };
    explicit MessageModel(QtNodes::MessageList *msgList, QObject *parent = nullptr);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    virtual ~MessageModel() override;
    void clear();

private slots:
    void insertRow(const int &row);
    void removeRow(const int &row);
    void update(const int &commId, int role);

private:
    QtNodes::MessageList *msgList_;

    std::vector<QMetaObject::Connection> metaConn_;
};
} // namespace dscontrol
