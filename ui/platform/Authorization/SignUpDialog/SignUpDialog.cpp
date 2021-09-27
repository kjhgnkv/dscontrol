#include "SignUpDialog.h"
#include "ui_SignUpDialog.h"
#include "DatabaseModule/DatabaseModule.h"

#include <QLineEdit>

SignUpDialog::SignUpDialog()
    : ui_{new Ui::SignUpDialog{}}
{
    ui_->setupUi(this);

    connect(ui_->registerButton, &QPushButton::clicked, this, &SignUpDialog::signUp);
    connect(ui_->cancelButton, &QPushButton::clicked, this, &SignUpDialog::reject);
}

QString SignUpDialog::userName() const
{
    return ui_->userNameLineEdit->text();
}

QString SignUpDialog::password() const
{
    return ui_->passwordLineEdit->text();
}

void SignUpDialog::signUp()
{
    auto userName = ui_->userNameLineEdit->text();
    auto password = ui_->passwordLineEdit->text();

    auto query = "INSERT INTO Users (userName, password)"
                "VALUES ('" + userName + "', MD5('" + password + "'))";

    //dscontrol::DatabaseModule::query(query, 1);

    accept();
}
