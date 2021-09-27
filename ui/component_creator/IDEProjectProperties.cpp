#include "IDEProjectProperties.hpp"
#include "manager/IDEProjectCreator.hpp"
#include "ui_IDEProjectProperties.h"

#include <QFileDialog>

namespace dscontrol
{
IDEProjectProperties::IDEProjectProperties(QWidget* parent)
: QDialog(parent)
, ui_(new Ui::IDEProjectProperties)
{
    ui_->setupUi(this);
    #ifdef __linux
    ui_->rbVS->setEnabled(false);
    #endif
    ui_->lblPath->setText(QDir::currentPath());
}

IDEProjectProperties::~IDEProjectProperties()
{
    delete ui_;
}

void IDEProjectProperties::on_rbQtCreator_clicked()
{
}

void IDEProjectProperties::on_btnPath_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), QDir::currentPath(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui_->lblPath->setText(dir);
}

void IDEProjectProperties::on_btnOk_clicked()
{
    if (ui_->rbVS->isChecked())
    {
        type_ = IDEProjectCreator::VisualStudio;
    }
    else if (ui_->rbQtCreator->isChecked())
    {
        type_ = IDEProjectCreator::QtCreator;
    }
    else
    {
        type_ = IDEProjectCreator::Unknown;
    }
    name_ = ui_->edName->text();
    path_ = ui_->lblPath->text();

    accept();
}

void IDEProjectProperties::on_pushButton_clicked()
{
    reject();
}

int IDEProjectProperties::IDEType()
{
    return type_;
}

QString IDEProjectProperties::Name()
{
    return name_;
}

QString IDEProjectProperties::Path()
{
    return path_;
}
} // namespace dscontrol
