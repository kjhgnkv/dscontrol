#include "IDEPath.hpp"

#include "ui_IDEPath.h"
#include <QFileDialog>

IDEPath::IDEPath(const QString &vsPath, const QString &qtPath, QWidget* parent)
: QDialog(parent)
, ui(new Ui::IDEPath)
, vsPath_ {vsPath}
, qtPath_ {qtPath}
{
    ui->setupUi(this);
    ui->lblQtPath->setText(qtPath_);
    ui->lblVSPath->setText(vsPath_);
    #ifdef __linux
    ui->btnVSPath->setEnabled(false);
    #endif
}

IDEPath::~IDEPath()
{
    delete ui;
}

void IDEPath::on_btnQtPath_clicked()
{
    QString qtPath = QFileDialog::getOpenFileName(this, tr("Select Qt Creator executable"), qtPath_, "");
    if (!qtPath.isEmpty())
    {
        qtPath_ = qtPath;
        ui->lblQtPath->setText(qtPath_);
    }
}

void IDEPath::on_btnVSPath_clicked()
{
    QString vsPath = QFileDialog::getOpenFileName(this, tr("Select Visual Studio executable"), vsPath_,
                                                  tr("Applications (*.exe)"));
    if (!vsPath.isEmpty())
    {
        vsPath_ = vsPath;
        ui->lblVSPath->setText(vsPath_);
    }
}

void IDEPath::on_btnOk_clicked()
{
    accept();
}

void IDEPath::on_btnCancel_clicked()
{
    reject();
}

QString IDEPath::getQtPath()
{
    return qtPath_;
}

QString IDEPath::getVSPath()
{
    return vsPath_;
}
