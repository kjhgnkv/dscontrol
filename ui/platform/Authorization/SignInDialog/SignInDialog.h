#pragma once

#include <QDialog>

namespace Ui {
class SignInDialog;
}

class SignInDialog : public QDialog
{
public:
    SignInDialog();
    int userId() const;
    QString userName() const;

private:
    void signIn();
    void signIn(const QString &userName, const QString &password);
    void signUp();

private:
    Ui::SignInDialog *ui_;
    int userId_;
    QString userName_;
};
