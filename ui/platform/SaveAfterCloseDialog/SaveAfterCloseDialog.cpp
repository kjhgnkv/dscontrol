#include "SaveAfterCloseDialog.hpp"

#include "ui_SaveAfterCloseDialog.h"
#include <QPushButton>

SaveAfterCloseDialog::SaveAfterCloseDialog(QWidget* parent)
: QDialog(parent)
, ui(new Ui::SaveAfterCloseDialog)
{
    ui->setupUi(this);

    // Handler
    connect(ui->saveBtn, &QPushButton::clicked, this, [=]
    {
        close();
        setResult(0);
    });
    connect(ui->saveAsBtn, &QPushButton::clicked, this, [=]
    {
        close();
        setResult(1);
    });
    connect(ui->notSaveBtn, &QPushButton::clicked, this, [=]
    {
        close();
        setResult(2);
    });
    connect(ui->cancelBtn, &QPushButton::clicked, this, [=]
    {
        close();
        setResult(3);
    });
    connect(this, &SaveAfterCloseDialog::rejected, this, [=]
    {
        ui->cancelBtn->click();
    });
}

SaveAfterCloseDialog::~SaveAfterCloseDialog()
{
    delete ui;
}

void SaveAfterCloseDialog::changeEvent(QEvent* event)
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
