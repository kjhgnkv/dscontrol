#include "Confirmation.hpp"

#include "ui_Confirmation.h"
#include <QDebug>

Confirmation::Confirmation(QWidget* parent)
: QDialog(parent)
, ui_(new Ui::Confirmation)
{
    ui_->setupUi(this);

    prepareControlEvents();
}

Confirmation::~Confirmation()
{
    delete ui_;
}

void Confirmation::prepareControlEvents()
{
    connect(ui_->pushButtonClose, &QPushButton::clicked, [this]()
    {
        QDialog::reject();
    });

    connect(ui_->pushButtonUpdate, &QPushButton::clicked, [this]()
    {
        QDialog::accept();
    });
}