#include "ObjectAttributesDialog.hpp"

#include "manager/models/DSProxyModel.hpp"
#include "manager/models/ObjectConnectionModel.hpp"
#include "singletones/Facade.hpp"
#include "ui_ObjectAttributesDialog.h"
#include <QKeyEvent>
#include <QMessageBox>
#include <QStandardItem>

namespace dscontrol
{
ObjectAttributesDialog::ObjectAttributesDialog(const QUuid &id, QWidget* parent)
: QDialog(parent)
, ui_(new Ui::ObjectAttributesDialog)
, sourceId_ {id}
, currentComponent_ {}
{
    ui_->setupUi(this);

    model_ = QtNodes::Facade::Instance().model();
    sourceIdx_ = model_->nodeIndex(sourceId_);
    if (sourceIdx_.isValid())
    {
        ui_->lineName_->setText(model_->nodeCaption(sourceIdx_));
        ui_->lineId_->setText(sourceId_.toString());
        ui_->lineId_->setCursorPosition(0);
    }

    recvModel_ = new ObjectConnectionModel {sourceId_, QtNodes::Item::PortType::In, this};

    ui_->recvView_->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
    ui_->recvView_->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    ui_->recvView_->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui_->recvView_->resizeRowsToContents();
    ui_->recvView_->setFocusPolicy(Qt::NoFocus);
    ui_->recvView_->verticalHeader()->hide();

    DSProxyModel* proxyRecv = new DSProxyModel {this};

    filter_ = [&currentComponent_ = currentComponent_](const QModelIndex &index)
    {
        auto name = index.model()->index(index.row(), ObjectConnectionModel::Columns::Name).data(
        Qt::DisplayRole).toString();
        if (currentComponent_ == name || currentComponent_.isEmpty())
        {
            return true;
        }
        return false;
    };
    proxyRecv->setSourceModel(recvModel_);
    proxyRecv->setFilter(filter_);
    proxyRecv->setFilterKeyColumn(ObjectConnectionModel::Columns::Name);
    ui_->recvView_->setModel(proxyRecv);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////

    sendModel_ = new ObjectConnectionModel {sourceId_, QtNodes::Item::PortType::Out, this};

    ui_->sendView_->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
    ui_->sendView_->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    ui_->sendView_->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui_->sendView_->resizeRowsToContents();
    ui_->sendView_->verticalHeader()->hide();

    DSProxyModel* proxySend = new DSProxyModel {this};

    proxySend->setSourceModel(sendModel_);
    proxySend->setFilter(filter_);
    proxySend->setFilterKeyColumn(ObjectConnectionModel::Columns::Name);
    ui_->sendView_->setModel(proxySend);

    componentsModel_ = new QStandardItemModel {this};
    componentsModel_->setColumnCount(1);
    componentsModel_->setHeaderData(0, Qt::Horizontal, QString {tr("Name")});
    for (const auto &child : model_->nodeChildsInLowestLevel(sourceIdx_))
    {
        componentsModel_->appendRow(new QStandardItem {child.nodeCaption()});
    }

    ui_->componentView_->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
    ui_->componentView_->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    ui_->componentView_->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui_->componentView_->resizeRowsToContents();
    ui_->componentView_->verticalHeader()->hide();
    ui_->componentView_->setFocusPolicy(Qt::NoFocus);
    ui_->componentView_->setEditTriggers(QTableView::NoEditTriggers);

    ui_->componentView_->setModel(componentsModel_);

    connect(ui_->componentView_, &DSTableView::clicked,
            [&filter_ = filter_, &currentComponent_ = currentComponent_, componentView_ = ui_->componentView_, proxyRecv, proxySend](
            const QModelIndex &index)
            {
                if (auto selmodel = componentView_->selectionModel(); selmodel->selectedIndexes().contains(index))
                {
                    currentComponent_ = index.data().toString();
                }
                else
                {
                    currentComponent_ = "";
                }
                proxyRecv->setFilter(filter_);
                proxySend->setFilter(filter_);
            });

    connect(ui_->btnClose_, &QPushButton::clicked, this, &ObjectAttributesDialog::close);

    this->setStyleSheet(R"(
                        QDialog{
                            background: #33333A;
                        }
                        QLabel{
                            color: white;}
                        QHeaderView
                        {
                            font-size: 15px;
                            font-style: normal;
                            font-weight: normal;
                        border: none;
                        border-right: none;
                        }
                        QHeaderView::section {
                        background-color: #252434;
                        border: none;
                        border-right: none;
                        color: #FFFFFF;
                        
                        }
                        QPushButton
                        {
                        background: #EB5757;
                        border: 1px solid #3B3E3D;
                        border-radius: 10px;

                        font-size: 18px;
                        line-height: 23px;
                        

                        color: #FFFFFF;
                        }
                        QPushButton::hover
                        {
                        background: #F87E7E;
                        border: 1px solid #3B3E3D;
                        border-radius: 10px;
                        }

                        QTableView#componentView_::item
                        {
                            background: #484853;
                            font-size: 14px;
                            color: #FFFFFF;
                            
                        }
                        QTableView#componentView_::item:hover
                        {
                            background: #595866;
                        }
                        QTableView#componentView_::item:selected
                        {
                          background: #7B59FF;
                        }
                        )");
}

ObjectAttributesDialog::~ObjectAttributesDialog()
{
    delete ui_;
}

void ObjectAttributesDialog::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
    {
        this->close();
    }
}

} // namespace dscontrol
