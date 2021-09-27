#include "ConnectionCreatorDialog.hpp"

#include "DSMessageBox/DSMessageBox.hpp"

#include "nodeeditor/lists/Communication.hpp"
#include "nodeeditor/lists/CommunicationList.hpp"
#include "nodeeditor/lists/MessageList.hpp"

#include "manager/models/AllConnectionModel.hpp"
#include "manager/models/DSProxyModel.hpp"

#include "manager/models/MessageModel.hpp"
#include "manager/models/SingleTinyMapModel.hpp"
#include "manager/models/TinyMapModel.hpp"

#include "nodeeditor/singletones/Facade.hpp"
#include "nodeeditor/singletones/MenuBuilder.h"
#include "ui_ConnectionCreatorDialog.h"

#include <QDebug>
#include <QMenu>

namespace dscontrol
{
ConnectionCreatorDialog::ConnectionCreatorDialog(const QUuid &id, QWidget* parent)
: QDialog(parent)
, ui_(new Ui::ConnectionCreatorDialog)
, isServer_{true}
, id_ {id}
, type_ {QtNodes::Item::PortType::None}
{
    ui_->setupUi(this);

    qDebug() << "ConnectionCreatorDialog: constructor";

    setWindowFlags(Qt::Window | Qt::WindowContextHelpButtonHint | Qt::WindowCloseButtonHint);

    ui_->horizontalLayout->setContentsMargins({813 + ui_->configPageBtnConfig_->width(), 0, 0, 0});
    ui_->configPageBtnConfig_->setVisible(false);
    ui_->receiverTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    ui_->senderTable_->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(ui_->configPageBtnConfig_, &QPushButton::clicked, this, [=]()
    {
        if (ui_->receiverTable_->selectionModel()->selectedRows().isEmpty() && ui_->senderTable_->selectionModel()->selectedRows().isEmpty())
        {
            DSMessageBox::warning(this, tr("Error"), tr("error"));
            return;
        }
        else if (!ui_->receiverTable_->selectionModel()->selectedRows().isEmpty())
        {
            this->recvConfigClicked();
        }
        else if (!ui_->senderTable_->selectionModel()->selectedRows().isEmpty())
        {
            this->sendConfigClicked();
        }
    });

    connect(ui_->btnRm_, &QPushButton::clicked, this, &ConnectionCreatorDialog::removeClicked);

    msgModel_ = new MessageModel {QtNodes::Facade::Instance().messageList(), this};
    nodesModel_ = QtNodes::Facade::Instance().model();
    helperModel_ = new TinyMapModel {{}, {}, this};
    helperModelComponents_ = new SingleTinyMapModel {{}, {}, this};
    connModel_ = new AllConnectionModel {id, ui_->allConnTable_};

    index_ = nodesModel_->nodeIndex(id_);

    messIn_ = nodesModel_->nodeMessages(index_, QtNodes::Item::PortType::In);
    messOut_ = nodesModel_->nodeMessages(index_, QtNodes::Item::PortType::Out);

    this->setWindowTitle(QString {tr("Create connection(%1)")}.arg(nodesModel_->nodeCaption(index_)));

    ui_->allConnTable_->setModel(connModel_);

    ui_->allConnTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
    ui_->allConnTable_->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    ui_->allConnTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    ui_->allConnTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui_->allConnTable_->setFocusPolicy(Qt::NoFocus);
    ui_->allConnTable_->setColumnHidden(AllConnectionModel::Columns::Uuid, true);
    ui_->allConnTable_->verticalHeader()->hide();
    ui_->allConnTable_->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui_->allConnTable_, &QTableView::customContextMenuRequested,
            [connModel_ = connModel_, allConnTable_ = ui_->allConnTable_](const QPoint &pos)
            {
                auto viewIndex = allConnTable_->indexAt(pos);
                if (viewIndex.isValid())
                {
                    auto modelIndex = connModel_->index(viewIndex.row(), AllConnectionModel::Columns::Uuid);
                    if (modelIndex.isValid())
                    {
                        auto connectionUuid = modelIndex.data().toUuid();
                        if (!connectionUuid.isNull())
                        {
                            auto connections = QtNodes::Facade::Instance().model()->connections();
                            auto foundedConnection = std::find_if(connections.begin(), connections.end(),
                                                                  [=](const auto &connection)
                                                                  {
                                                                      return connection.first.id_ == connectionUuid;
                                                                  });
                            if (connections.end() != foundedConnection)
                            {
                                auto connection = foundedConnection->second;
                                auto menu = QtNodes::MenuBuilder::allConnectionsViewMenu(connection);
                                if (nullptr != menu)
                                {
                                    menu->exec(allConnTable_->viewport()->mapToGlobal(pos));
                                    menu->deleteLater();
                                }
                            }
                        }
                    }
                }
            });

    connect(ui_->btnCfgNew_, &QPushButton::clicked, this, &ConnectionCreatorDialog::addNewClicked);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////

    ui_->receiverTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
    ui_->receiverTable_->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    ui_->receiverTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui_->receiverTable_->resizeRowsToContents();
    ui_->receiverTable_->setFocusPolicy(Qt::NoFocus);
    ui_->receiverTable_->verticalHeader()->hide();

    DSProxyModel* proxyRecv = new DSProxyModel {this};

    // TODO bad practic. need delete one filter
    filterRecv_ = [&messIn = messIn_](const QModelIndex &index)
    {
        auto id = index.data(Qt::DisplayRole).toInt();
        auto it = std::find_if(messIn.begin(), messIn.end(), [id](const auto &value)
        {
            if (value.messId_ == id)
            {
                return true;
            }

            return false;
        });

        if (it != messIn.end())
            return true;
        return false;
    };

    proxyRecv->setSourceModel(msgModel_);
    proxyRecv->setFilter(filterRecv_);
    ui_->receiverTable_->setModel(proxyRecv);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////

    ui_->senderTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
    ui_->senderTable_->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    ui_->senderTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui_->senderTable_->resizeRowsToContents();
    ui_->senderTable_->verticalHeader()->hide();

    DSProxyModel* proxySend = new DSProxyModel {this};

    filterSend_ = [&messOut = messOut_](const QModelIndex &index)
    {
        auto id = index.data(Qt::DisplayRole).toInt();
        auto it = std::find_if(messOut.begin(), messOut.end(), [id](const auto &value)
        {
            if (value.messId_ == id)
            {
                return true;
            }

            return false;
        });

        if (it != messOut.end())
            return true;
        return false;
    };

    proxySend->setSourceModel(msgModel_);
    proxySend->setFilter(filterSend_);
    ui_->senderTable_->setModel(proxySend);

    ui_->commTree->header()->hide();
    ui_->commTree->setRootIsDecorated(false);
    ui_->commTree->setItemsExpandable(false);
    ui_->commTree->setSelectionMode(QAbstractItemView::NoSelection);
    ui_->commTree->setFocusPolicy(Qt::NoFocus);
    ui_->commTree->header()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);

    ui_->componentsTree->header()->hide();
    ui_->componentsTree->setRootIsDecorated(false);
    ui_->componentsTree->setItemsExpandable(false);
    ui_->componentsTree->setSelectionMode(QAbstractItemView::NoSelection);
    ui_->componentsTree->setFocusPolicy(Qt::NoFocus);
    ui_->componentsTree->header()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
    ui_->componentsTree->setItemsExpandable(false);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////

    initQSS();

    connect(ui_->receiverTable_->selectionModel(), &QItemSelectionModel::selectionChanged,
            [senderTable_ = ui_->senderTable_, model = ui_->receiverTable_->selectionModel(), button = ui_->configPageBtnConfig_, layout = ui_->horizontalLayout](
            const QItemSelection &selected)
            {
                if (!selected.isEmpty())
                {
                    senderTable_->selectionModel()->clearSelection();
                }
                for (auto idx : model->selectedRows(MessageModel::Columns::CommNames))
                {
                    if (!idx.data().isNull())
                    {
                        layout->setContentsMargins({807, 0, 0, 0});
                        button->show();
                        return;
                    }
                    button->hide();
                    layout->setContentsMargins({813 + button->width(), 0, 0, 0});
                }
            });

    connect(ui_->senderTable_->selectionModel(), &QItemSelectionModel::selectionChanged,
            [receiverTable_ = ui_->receiverTable_, model = ui_->senderTable_->selectionModel(), button = ui_->configPageBtnConfig_, layout = ui_->horizontalLayout](
            const QItemSelection &selected)
            {
                if (!selected.isEmpty())
                    receiverTable_->selectionModel()->clearSelection();

                for (auto idx : model->selectedRows(MessageModel::Columns::CommNames))
                {
                    if (!idx.data().isNull())
                    {
                        layout->setContentsMargins({807, 0, 0, 0});
                        button->show();

                        return;
                    }
                    button->hide();
                    layout->setContentsMargins({813 + button->width(), 0, 0, 0});
                }
            });

    connect(ui_->commTree, &QTreeView::clicked, [helperModel_ = helperModel_](const QModelIndex &index)
    {
        auto val = index.data(
        Qt::CheckStateRole) == Qt::CheckState::Checked ? Qt::CheckState::Unchecked : Qt::CheckState::Checked;
        helperModel_->setData(index, val, Qt::EditRole);
    });
    connect(ui_->componentsTree, &QTreeView::clicked,
            [helperModelComponents_ = helperModelComponents_](const QModelIndex &index)
            {
                auto val = index.data(
                Qt::CheckStateRole) == Qt::CheckState::Checked ? Qt::CheckState::Unchecked : Qt::CheckState::Checked;
                helperModelComponents_->setData(index, val, Qt::EditRole);
            });

    connect(ui_->btnConfigureComplete_, &QPushButton::clicked, this, &ConnectionCreatorDialog::ConfigureApplyClicked);

    connect(ui_->btnCancel_, &QPushButton::clicked, this, &ConnectionCreatorDialog::close);
    connect(ui_->configPageBtnCancel_, &QPushButton::clicked, this, &ConnectionCreatorDialog::close);
    connect(ui_->btnCancel_1, &QPushButton::clicked, this, &ConnectionCreatorDialog::close);
}

ConnectionCreatorDialog::~ConnectionCreatorDialog()
{
    qDebug() << "ConnectionCreatorDialog: destructor";
    delete ui_;
}

void ConnectionCreatorDialog::recvConfigClicked()
{
    if (!ui_->receiverTable_->selectionModel()->selectedRows().size())
    {
        qWarning() << "selected recv are empty";
        return;
    }

    type_ = QtNodes::Item::PortType::In;

    auto nameIndex = ui_->receiverTable_->selectionModel()->selectedRows(MessageModel::Columns::Name).at(0);
    auto idIndex = ui_->receiverTable_->selectionModel()->selectedRows(MessageModel::Columns::Id).at(0);
    auto commIndex = ui_->receiverTable_->selectionModel()->selectedRows(MessageModel::Columns::CommNames).at(0);

    if (!nameIndex.isValid() && !idIndex.isValid() && !commIndex.isValid())
    {
        qWarning() << "non valid indexes";
        DSMessageBox::critical(this, tr("Error"), tr("error"));
        return;
    }

    msgId_ = idIndex.data(Qt::DisplayRole).toString().toInt();
    ui_->lineMessId_->setText(QString::number(msgId_));
    ui_->lineMessName_->setText(nameIndex.data(Qt::DisplayRole).toString());
    ui_->lblSetMethod_->setText(tr("Receive from: "));

    helperModel_->setAllCommNames(commIndex.data(Qt::DisplayRole).toString().split("\n"));
    helperModel_->setCreatedCommNames(connModel_->getConnectionAssociate(QtNodes::Item::PortType::In));
    ui_->commTree->setModel(helperModel_);

    auto components = nodesModel_->nodes(QtNodes::Item::NodeType::ComponentNode);
    QStringList lstOfComponents;

    for (const auto &component : components)
    {
        auto messages = nodesModel_->nodeMessages(component, QtNodes::Item::PortType::Out);
        auto found = std::find_if(messages.begin(), messages.end(), [&msgId_ = msgId_](const auto &value)
        {
            if (value.messId_ == msgId_)
                return true;
            return false;
        });
        if (found != messages.end())
        {
            auto path = QtNodes::Facade::Instance().generateComponentOwnership(component);
            lstOfComponents.push_back(path);
            acceptedComponents_.insert({path, component});
        }
    }
    helperModelComponents_->setAllCommNames(lstOfComponents);
    ui_->componentsTree->setModel(helperModelComponents_);

    ui_->stackedWidget->setCurrentIndex(2);
    qDebug() << "ConnectionCreatorDialog: receiver click accept";
}

void ConnectionCreatorDialog::sendConfigClicked()
{
    if (!ui_->senderTable_->selectionModel()->selectedRows().size())
    {
        qWarning() << "selected recv are empty";
        return;
    }

    type_ = QtNodes::Item::PortType::Out;

    auto nameIndex = ui_->senderTable_->selectionModel()->selectedRows(MessageModel::Columns::Name).at(0);
    auto idIndex = ui_->senderTable_->selectionModel()->selectedRows(MessageModel::Columns::Id).at(0);
    auto commIndex = ui_->senderTable_->selectionModel()->selectedRows(MessageModel::Columns::CommNames).at(0);

    if (!nameIndex.isValid() && !idIndex.isValid() && !commIndex.isValid())
    {
        qWarning() << "non valid indexes";
        DSMessageBox::critical(this, tr("Error"), tr("error"));
        return;
    }

    msgId_ = idIndex.data(Qt::DisplayRole).toString().toInt();
    ui_->lineMessId_->setText(QString::number(msgId_));
    ui_->lineMessName_->setText(nameIndex.data(Qt::DisplayRole).toString());
    ui_->lblSetMethod_->setText(tr("Send to: "));

    helperModel_->setAllCommNames(commIndex.data(Qt::DisplayRole).toString().split("\n"));
    helperModel_->setCreatedCommNames(connModel_->getConnectionAssociate(QtNodes::Item::PortType::Out));
    ui_->commTree->setModel(helperModel_);

    auto components = nodesModel_->nodes(QtNodes::Item::NodeType::ComponentNode);
    QStringList lstOfComponents;

    for (const auto &component : components)
    {
        auto messages = nodesModel_->nodeMessages(component, QtNodes::Item::PortType::In);
        auto found = std::find_if(messages.begin(), messages.end(), [&msgId_ = msgId_](const auto &value)
        {
            if (value.messId_ == msgId_)
                return true;
            return false;
        });
        if (found != messages.end())
        {
            auto path = QtNodes::Facade::Instance().generateComponentOwnership(component);
            lstOfComponents.push_back(path);
            acceptedComponents_.insert({path, component});
        }
    }
    helperModelComponents_->setAllCommNames(lstOfComponents);
    ui_->componentsTree->setModel(helperModelComponents_);

    ui_->stackedWidget->setCurrentIndex(2);
    qDebug() << "ConnectionCreatorDialog: sender click accept";
}

void ConnectionCreatorDialog::addNewClicked()
{
    qDebug() << "ConnectionCreatorDialog: add new";
    ui_->lineName_->setText(nodesModel_->nodeCaption(index_));
    ui_->lineId_->setText(id_.toString());

    ui_->stackedWidget->setCurrentIndex(1);
}

void ConnectionCreatorDialog::ConfigureApplyClicked()
{
    qDebug() << "ConnectionCreatorDialog: configure clicked";
    if (helperModel_->getLastCommNames().isEmpty())
    {
        qWarning() << "empty selected comm's";
        DSMessageBox::critical(this, tr("Error"), tr("Select communication"));
        return;
    }

    if (helperModelComponents_->getLastCommNames().isEmpty())
    {
        qWarning() << "empty selected components";
        DSMessageBox::critical(this, tr("Error"), tr("Select \"") + ui_->lblSetMethod_->text() + "\"");
        return;
    }

    auto found = std::find_if(acceptedComponents_.begin(), acceptedComponents_.end(),
                              [&helperModelComponents_ = helperModelComponents_](const auto &val)
                              {
                                  if (val.first == helperModelComponents_->getLastCommNames().at(0))
                                  {
                                      return true;
                                  }
                                  return false;
                              });

    if (found != acceptedComponents_.end())
    {
        auto comms = helperModel_->getLastCommNames();
        for (const auto &comm : helperModel_->getCreatedCommNames())
        {
            comms.removeOne(comm);
        }

        bool ret;
        for (const auto &comm : comms)
        {
            auto lst = QtNodes::Facade::Instance().communicationList()->communications();
            auto fnd = std::find_if(lst.begin(), lst.end(), [=](const auto &val)
            {
                if (val->commName_ == comm)
                {
                    return true;
                }
                return false;
            });
            if (fnd == lst.end())
            {
                qWarning() << "can't find comm's";
                return;
            }

            auto commId = fnd->get()->id_;
            QtNodes::Facade::Instance().communicationList();
            auto serverIdx = isServer_ ? index_ : found->second;
            switch (type_)
            {
                case QtNodes::Item::PortType::In:
                {
                    ret = QtNodes::Facade::Instance().createConnection(found->second, index_, serverIdx, commId, msgId_);
                    break;
                }
                case QtNodes::Item::PortType::Out:
                {
                    ret = QtNodes::Facade::Instance().createConnection(index_, found->second, serverIdx, commId, msgId_);
                    break;
                }
                default:
                {
                    ret = false;
                }
            }
            if (!ret)
            {
                qWarning() << "cannot create connection";
                DSMessageBox::critical(this, tr("error"), tr("error123123"));
            }
        }
    }
    this->close();
}

void ConnectionCreatorDialog::removeClicked()
{
    qDebug() << "ConnectionCreatorDialog: remove clicked";
    if (!ui_->allConnTable_->selectionModel()->selectedRows().size())
    {
        qWarning() << "selected are empty";
        return;
    }

    auto idIndex = ui_->allConnTable_->selectionModel()->selectedRows(dscontrol::AllConnectionModel::Columns::Uuid).at(
    0);

    if (!idIndex.isValid())
    {
        qWarning() << "non valid indexes";
        DSMessageBox::critical(this, tr("Error"), tr("error"));
        return;
    }

    if (!QtNodes::Facade::Instance().removeConnection(index_, idIndex.data(Qt::DisplayRole).toUuid()))
    {
        qWarning() << "cannot remove connection";
        DSMessageBox::critical(this, tr("Error"), tr("error"));
        return;
    }
}

void ConnectionCreatorDialog::initQSS()
{
    this->setStyleSheet(R"(
                        QDialog {background:  #33333A;}
                        QLabel{
                        color: white;}
                        QPushButton {
                        background-color: #605F6E;
                        border: 1px solid #3B3E3D;
                        border-radius: 10px;
                        color: #FFFFFF;
                        font-size: 16px;
                        }
                        QPushButton::hover
                        {
                        background: #605F6E;
                        border: 1px solid #7B59FF;
                        }
                        QPushButton::pressed
                        {
                        background: #7B59FF;
                        }
                        QPushButton#btnCfgNew_{
                        background-color: #27AE60;
                        }
                        QPushButton#btnCfgNew_::hover{
                        background: #46CE7F;
                        }
                        QPushButton#btnConfigureComplete_{
                        background-color: #27AE60;
                        }
                        QPushButton#btnCancel_{
                        background-color: #EB5757;
                        }
                        QPushButton#btnCancel_1{
                        background-color: #EB5757;
                        }
                        QPushButton#btnConfigureComplete_::hover{
                        background: #46CE7F;
                        border: 1px solid #3B3E3D;
                        }
                        QPushButton#btnCancel_::hover{
                        background: #F87E7E;
                        border: 1px solid #3B3E3D;
                        }
                        QPushButton#btnCancel_1::hover{
                        background: #F87E7E;
                        border: 1px solid #3B3E3D;
                        }

                        QLabel{
                        color: #FFFFFF;
                        }
                        QLabel#lblName_{
                        font-size: 14px
                        }
                        QLabel#lblId_{
                        font-size: 14px
                        }
                        QLabel#lblMessName_{
                        font-size: 14px
                        }
                        QLabel#lblMessId_{
                        font-size: 14px
                        }
                        QLabel#lblSender_{
                        font-size: 18px
                        }
                        QLabel#lblComm_{
                        font-size: 18px
                        }
                        QLabel#lblSetMethod_{
                        font-size: 18px
                        }
                        QLabel#lblReceiver_{
                        font-size: 18px
                        }

                        QFrame#frame{
                        background: #484853;
                        border: 1px solid #33333A;
                        }
                        QFrame#frame_2{
                        background: #484853;
                        border: 1px solid #33333A;
                        }
                        QPushButton#configPageBtnConfig_{
                        width: 88px;
                        height: 28px;
                        background: #605F6E;
                        border: 1px solid #3B3E3D;
                        border-radius: 10px;
                        }
                        QPushButton#configPageBtnConfig_::hover{
                        background: #605F6E;
                        border: 1px solid #7B59FF;
                        }
                        QPushButton#configPageBtnConfig_::pressed{
                        background: #7B59FF;
                        }

                        QTreeView
                        {
                        background: #484853;
                        border: 1px solid #33333A;
                        }
                        QCheckBox::indicator {
                            width: 12px;
                            height: 12px;
                        }
                        QTreeView::indicator:unchecked{
                            image: url(:/images/resources/images/unchecked.png)
                        }
                        QTreeView::indicator:checked{
                            image: url(:/images/resources/images/checked.png)
                        }
                        QTreeView
                        {
                        background: #484853;
                        border: 1px solid #33333A;
                        }
                        QTreeView::item
                        {
                        background: #605F6E;
                        border: 1px solid #33333A;
                        padding: 13px 13px;
                        color: #FFFFFF;
                        font-size: 14px;
                        }
                        QTreeView::item:selected
                        {
                        background: #484853;
                        border: 1px solid #33333A;
                        }
                        QTreeView::item:hover
                        {
                        background: #595866;
                        border: 1px solid #33333A;
                        }
                        QPushButton#configPageBtnCancel_{
                        width: 88px;
                        height: 28px;
                        background: #EB5757;
                        border: 1px solid #3B3E3D;
                        border-radius: 10px;
                        font-size: 18px;
                        }
                        QPushButton#configPageBtnCancel_::hover{
                        background: #F87E7E;
                        border: 1px solid #3B3E3D;
                        }
                        )");
}

void ConnectionCreatorDialog::changeEvent(QEvent* event)
{
    if (event)
    {
        switch (event->type())
        {
            case QEvent::LanguageChange:
            {
                ui_->retranslateUi(this);
                break;
            }
            default:
            {
                break;
            }
        }
        QDialog::changeEvent(event);
    }
}

} // namespace dscontrol
