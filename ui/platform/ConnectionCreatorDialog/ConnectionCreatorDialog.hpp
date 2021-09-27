#pragma once

#include "nodeeditor/lists/Message.hpp"
#include "nodeeditor/common/nodeeditor_globals.hpp"
#include "nodeeditor/nodes/NodeIndex.hpp"

#include <QDialog>
#include <QUuid>
#include <functional>

namespace QtNodes
{
class DataFlowModel;
class MessageList;
class NodeIndex;
} // namespace QtNodes

namespace Ui
{
class ConnectionCreatorDialog;
}

namespace dscontrol
{
class SingleTinyMapModel;
class TinyMapModel;
class MessageModel;
class AllConnectionModel;

/*!
\brief Create connection dialog class
 */
class ConnectionCreatorDialog : public QDialog
{
Q_OBJECT

public:
    /*!
    \param id Uuid of component for which connection should be created
     */
    explicit ConnectionCreatorDialog(const QUuid &id, QWidget* parent = nullptr);
    ~ConnectionCreatorDialog();

public slots:
    void recvConfigClicked();
    void sendConfigClicked();

    void addNewClicked();

    void ConfigureApplyClicked();
    void removeClicked();

private:
    void initQSS();

private:
    Ui::ConnectionCreatorDialog* ui_;
    bool isServer_;
    QUuid id_;
    int msgId_;
    MessageModel* msgModel_;
    QtNodes::MessageList* msgList_;
    QtNodes::DataFlowModel* nodesModel_;
    std::list<QtNodes::Message> messIn_;
    std::list<QtNodes::Message> messOut_;
    QtNodes::NodeIndex index_;
    TinyMapModel* helperModel_;
    SingleTinyMapModel* helperModelComponents_;
    QtNodes::NodeIndex destIndex_;
    std::map<QString, QtNodes::NodeIndex> acceptedComponents_;
    QtNodes::Item::PortType type_;
    AllConnectionModel* connModel_;
    std::function<bool(const QModelIndex &sourceIndex)> filterRecv_;
    std::function<bool(const QModelIndex &sourceIndex)> filterSend_;

    // QWidget interface
protected:
    void changeEvent(QEvent* event);
};
} // namespace dscontrol
