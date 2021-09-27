#include "NetworkWidget.hpp"

#include "models/AbstractItemModel.hpp"
#include "node/NodeItemSniffer.hpp"
#include "ui_NetworkWidget.h"
#include <QDebug>
#include <QFile>

NetworkWidget* NetworkWidget::networkWidget_ = nullptr;

NetworkWidget::NetworkWidget(QWidget* parent)
: QDialog(parent)
, ui_(new Ui::NetworkWidget)
{
    ui_->setupUi(this);

    ui_->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui_->tableView->horizontalHeader()->setStretchLastSection(false);
    ui_->tableView->verticalHeader()->hide();
    ui_->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui_->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui_->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    model_ = new AbstractItemModel(new NodeItemSniffer());
    model_->appendTitle({"PC", "Source", "Destination", "Protocol", "Length", "Info"});
    ui_->tableView->setModel(model_);

    connect(ui_->pushButtonClose, &QPushButton::clicked, this, [&]
    {
        QDialog::reject();
    });
}

NetworkWidget::~NetworkWidget()
{
    delete ui_;
}

NetworkWidget* NetworkWidget::init(QWidget* parent)
{
    if (networkWidget_ == nullptr)
    {
        networkWidget_ = new NetworkWidget {parent};
    }

    return networkWidget_;
}

void NetworkWidget::updateData(const QString &array)
{
    AbstractNodeItem* item = new NodeItemSniffer(QVariant(array));
    model_->insert(item);
}
