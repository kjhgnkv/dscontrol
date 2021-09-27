#include "AfterSuccessfullySavedDialog.hpp"

#include "ui_AfterSuccessfullySavedDialog.h"

namespace dscontrol
{
AfterSuccessfullySavedDialog::AfterSuccessfullySavedDialog(QWidget* parent)
: QDialog(parent)
, ui(new Ui::AfterSuccessfullySavedDialog)
{
    ui->setupUi(this);
    connect(ui->pushButton, &QPushButton::clicked, this, &QDialog::accept);
}

AfterSuccessfullySavedDialog::~AfterSuccessfullySavedDialog()
{
    delete ui;
}

void AfterSuccessfullySavedDialog::changeEvent(QEvent* event)
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
} // namespace dscontrol
