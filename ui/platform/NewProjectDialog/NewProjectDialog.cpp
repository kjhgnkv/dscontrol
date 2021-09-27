#include "NewProjectDialog.hpp"

#include "DSMessageBox/DSMessageBox.hpp"
#include "custom/DSLineEdit.hpp"
#include "ui_NewProjectDialog.h"
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QRegExpValidator>

namespace dscontrol
{
NewProjectDialog::NewProjectDialog(QWidget* parent)
: QDialog(parent)
, ui_(new Ui::NewProjectDialog)
{
    qDebug() << "NewProjectDialog: konstructor";
    ui_->setupUi(this);

    setWindowFlags(Qt::WindowCloseButtonHint);

    ui_->lineName_->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9_]{0,25}"), this));
    ui_->linePath_->setText(QDir::currentPath());

    connect(ui_->linePath_, &dscontrol::DSLineEdit::toolClicked, this, &NewProjectDialog::setPath);
    connect(ui_->btnCreateProject_, &QPushButton::clicked, this, &NewProjectDialog::createProject);

    ui_->lineName_->setFocus();
}

NewProjectDialog::~NewProjectDialog()
{
    qDebug() << "NewProjectDialog: destructor";
    delete ui_;
}

QString NewProjectDialog::getProjectName()
{
    return ui_->lineName_->text();
}

QString NewProjectDialog::getProjectPath()
{
    return ui_->linePath_->text();
}

void NewProjectDialog::createProject()
{
    qDebug() << "NewProjectDialog: try accept";
    bool dirExist = false;
    if (ui_->linePath_->text().length() != 0)
    {
        if (QDir(ui_->linePath_->text()).exists())
        {
            dirExist = true;
        }
        else
        {
            if (QDir(ui_->linePath_->text()).mkpath(ui_->linePath_->text()))
            {
                dirExist = true;
            }
            else
            {
                qDebug() << "NewProjectDialog: can not be accepted, because "
                            "directory "
                            "is not valid";
                DSMessageBox::warning(0, tr("Invalid directory"), tr("Can't create directory check permition "
                                                                     "rules\nDirectory not "
                                                                     "exist\nPlease edit Path"));
            }
        }
    }
    else
    {
        qDebug() << "NewProjectDialog: can not be accepted, because directory "
                    "is not valid";
        DSMessageBox::warning(0, tr("Invalid directory"), tr("No path\nPlease enter Path"));
    }

    if (dirExist)
    {
        if (ui_->lineName_->text().isEmpty())
        {
            qDebug() << "NewProjectDialog: can not be accepted, because "
                        "project name "
                        "is not valid";
            DSMessageBox::warning(0, tr("Invalid project name"), tr("Enter project name"));
        }
        else
        {
            qDebug() << "NewProjectDialog accepted";
            this->accept();
        }
    }
}

void NewProjectDialog::setPath()
{
    ui_->linePath_->setText(QDir::toNativeSeparators(QFileDialog::getExistingDirectory()));
}

void NewProjectDialog::changeEvent(QEvent* event)
{
    if (event)
    {
        switch (event->type())
        {
            case QEvent::LanguageChange:
            {
                ui_->retranslateUi(this);
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
