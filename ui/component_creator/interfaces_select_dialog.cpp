// interfaces_select_dialog.cpp

#include "interfaces_select_dialog.hpp"
#include "interface_store_model.hpp"
#include "ui_interfaces_select_dialog.h"

#include <QSortFilterProxyModel>
#include <QTreeView>

interfaces_select_dialog::interfaces_select_dialog(QSortFilterProxyModel* model, ::QWidget* parent)
: QDialog {parent}
, ui {new Ui::interfaces_select_dialog}
{
    ui->setupUi(this);
    ui->treeView->setModel(model);
    // resize first column to context
    ui->treeView->resizeColumnToContents(0);
    connect(ui->lineEdit, &::QLineEdit::textChanged, model, &::QSortFilterProxyModel::setFilterWildcard);
    connect(ui->select, &::QPushButton::clicked, this, [=]()
    {
        emit(selected_index(ui->treeView->currentIndex()));
    });

    ui->treeView->header()->setSectionResizeMode(QHeaderView::Stretch);
    ui->treeView->setRootIsDecorated(false);
}

interfaces_select_dialog::~interfaces_select_dialog()
{
    delete ui;
}

void interfaces_select_dialog::changeEvent(QEvent* event)
{
    if (event)
    {
        switch (event->type())
        {
            case QEvent::LanguageChange:
            {
                ui->retranslateUi(this);
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
