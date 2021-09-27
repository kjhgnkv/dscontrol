#include "addcustomwidgetdialog.h"
#include "ui_addcustomwidgetdialog.h"

#include "manager/CustomDesignerPluginGenerator.h"

#include <QFileDialog>
#include <QFileInfo>

AddCustomWidgetDialog::AddCustomWidgetDialog(QWidget *parent) :
    QDialog(parent),
    ui_(new Ui::AddCustomWidgetDialog)
{
    ui_->setupUi(this);

    connect(ui_->btnChooseFile, &QPushButton::clicked, this, &AddCustomWidgetDialog::selectFile);
    connect(ui_->btnAdd, &QPushButton::clicked, this, &AddCustomWidgetDialog::addClicked);
}

AddCustomWidgetDialog::~AddCustomWidgetDialog()
{
    delete ui_;
}

void AddCustomWidgetDialog::selectFile()
{
    path_ = QFileDialog::getOpenFileName(this, "Select ui file", "", "*.ui");
    ui_->lineEdit->setText(path_);
}

void AddCustomWidgetDialog::addClicked()
{
    if( path_.isEmpty() )
    {
        return;
    }

    QFileInfo info{path_};

    auto filename = info.fileName().split(".", QString::SkipEmptyParts).at(0);

    dscontrol::CustomDesignerPluginGenerator::generate(filename, path_);
}
