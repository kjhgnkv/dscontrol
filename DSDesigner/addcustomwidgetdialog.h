#pragma once

#include <QDialog>

namespace Ui {
class AddCustomWidgetDialog;
}

class AddCustomWidgetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddCustomWidgetDialog(QWidget *parent = nullptr);
    ~AddCustomWidgetDialog();

private slots:
    void selectFile();
    void addClicked();

private:
    Ui::AddCustomWidgetDialog *ui_;

    QString path_;
};

