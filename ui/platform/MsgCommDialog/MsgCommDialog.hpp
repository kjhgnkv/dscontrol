#pragma once

#include <QDialog>
#include <QStringListModel>

namespace Ui
{
class MsgCommDialog;
}

namespace QtNodes
{
class MessageList;
class CommunicationList;
} // namespace QtNodes

namespace dscontrol
{
class TinyMapModel;

/*!
\brief Class representing dialog to make correlations between messages and
 communications
 */
class MsgCommDialog : public QDialog
{
Q_OBJECT

public:
    explicit MsgCommDialog(QtNodes::CommunicationList* commList, QtNodes::MessageList* msgList,
                           const QModelIndex &index, QWidget* parent = nullptr);
    ~MsgCommDialog() override;

protected:
    void closeEvent(QCloseEvent* event) override;
    void initQSS();

private slots:
    void removeComm(QString name);

private:
    bool deleteExistingConnections(const QString &commName);

private:
    QStringList savedCommNames;
    Ui::MsgCommDialog* ui;
    TinyMapModel* helperModel;
    QtNodes::CommunicationList* commList_;
    QtNodes::MessageList* msgList_;
    QModelIndex msgModelIndex;

    // QWidget interface
protected:
    void changeEvent(QEvent* event);
};
} // namespace dscontrol
