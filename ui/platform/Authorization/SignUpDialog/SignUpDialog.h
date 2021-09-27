#pragma once

#include <QDialog>

namespace Ui {
class SignUpDialog;
}

class SignUpDialog : public QDialog
{
public:
    SignUpDialog();
    QString userName() const;
    QString password() const;

private:
    void signUp();

private:
    Ui::SignUpDialog *ui_;
};
