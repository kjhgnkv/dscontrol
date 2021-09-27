#include "SignInDialog.h"
#include "ui_SignInDialog.h"
#include "Authorization/SignUpDialog/SignUpDialog.h"
#include "DatabaseModule/DatabaseModule.h"

#include <QLineEdit>

SignInDialog::SignInDialog()
    : ui_{new Ui::SignInDialog{}}
    , userId_{-1}
{
    ui_->setupUi(this);

    connect(ui_->signInButton, &QPushButton::clicked, this, QOverload<>::of(&SignInDialog::signIn));
    connect(ui_->signUpButton, &QPushButton::clicked, this, &SignInDialog::signUp);
    connect(ui_->cancelButton, &QPushButton::clicked, this, &SignInDialog::reject);
}

int SignInDialog::userId() const
{
    return userId_;
}

QString SignInDialog::userName() const
{
    return userName_;
}

void SignInDialog::signIn()
{
    auto userName = ui_->userNameLineEdit->text();
    auto password = ui_->passwordLineEdit->text();

    signIn(userName, password);
}

void SignInDialog::signIn(const QString &userName, const QString &password)
{
//    const auto &records = dscontrol::DatabaseModule::query(
//                "SELECT id FROM Users WHERE userName='" + userName +
//                "' AND password=MD5('" + password + "')", 1);

    QList<QList<QVariant>> records;

    if (records.isEmpty())
    {
        return;
    }
    auto firstRecord = records.at(0);
    if (firstRecord.isEmpty())
    {
        return;
    }
    userId_ = firstRecord.at(0).toInt();
    userName_ = userName;

    accept();
}

void SignInDialog::signUp()
{
    SignUpDialog dialog;
    if (dialog.exec())
    {
        auto userName = dialog.userName();
        auto password = dialog.password();

        signIn(userName, password);
    }
}
