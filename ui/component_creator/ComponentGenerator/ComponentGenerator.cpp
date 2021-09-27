#include "ComponentGenerator.h"
#include "ui_ComponentGenerator.h"
#include "global.h"
#include "wizard.h"
#include "generator.h"

#include <QFile>
#include <QString>
#include <QFileDialog>

ComponentGenerator::ComponentGenerator(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ComponentGenerator)
{
    ui->setupUi(this);
    Q_INIT_RESOURCE(templates);
}

ComponentGenerator::~ComponentGenerator()
{
    delete ui;
}

void ComponentGenerator::on_generateBtn_clicked()
{
    //TODO: !!! REWRITE COMPONENT GENERATION TO OOP+QT STYLE?
    gengetopt_args_info ai;

    std::string value;

    //project name
    value = ui->projectNameEdit->text().toStdString();

    ai.name_arg = static_cast<char*>(malloc(value.length() + 1));
    strncpy(ai.name_arg, value.c_str(), value.length() + 1);
    ai.name_given = 0;

    //library name
    value = ui->libraryNameEdit->text().toStdString();

    ai.library_name_arg = static_cast<char*>(malloc(value.length() + 1));
    strncpy(ai.library_name_arg, value.c_str(), value.length() + 1);
    ai.library_name_given = 0;

    //code directory
    value = ui->codeDirectoryEdit->text().toStdString();

    ai.output_directory_arg = static_cast<char*>(malloc(value.length() + 1));
    strncpy(ai.output_directory_arg, value.c_str(), value.length() + 1);
    ai.output_directory_given = 0;

    //dshost header dir
    value = ui->dshostHeadersEdit->text().toStdString();

    ai.include_directory_arg = static_cast<char*>(malloc(value.length() + 1));
    strncpy(ai.include_directory_arg, value.c_str(), value.length() + 1);
    ai.include_directory_given = 0;

        //component type
    switch(ui->comboBox->currentIndex())
    {
        case 0:
            ai.type_arg = type_arg_object;
            break;
        case 1:
            ai.type_arg = type_arg_protocol;
            break;
        case 2:
            ai.type_arg = type_arg_transport;
            break;
        case 3:
            ai.type_arg = type_arg_custom;
            break;
        default:
            ai.type_arg = type_arg_object;
            break;
    }
    ai.type_given = 0;
    ai.namespace_given = 0;

    //namespace
    value = ui->namespaceEdit->text().toStdString();

    ai.namespace_arg = static_cast<char*>(malloc(value.length() + 1));
    strncpy(ai.namespace_arg, value.c_str(), value.length() + 1);

    //qt support
    ai.qt_gui_given = ui->qtSupportCheckBox->isChecked();

    execute_wizard(&ai);

    bool checked = true;

    if (!ai.name_given)
    {
        checked = false;
       //notify user
    }

    if (!ai.library_name_given)
    {
        checked = false;
        //notify user
    }

    if (!ai.output_directory_given)
    {
        checked = false;
        //notify user
    }

    if (!ai.include_directory_given)
    {
        checked = false;
        //notify user
    }

    if (!ai.type_given)
    {
        checked = false;
        //notify user
    }

    if(ai.type_arg == type_arg_object)
    {
        if (!ai.namespace_given)
        {
            checked = false;
            //notify user
        }
    }

    if (!checked)
    {
        //notify? 
        return;
    }

    auto code = generate_component_code(&ai);

    if (!code)
    {
        accept();
    }
    else
    {
        //notify user
        return;
    }
}

void ComponentGenerator::on_cancelBtn_clicked()
{
    reject();
}

void ComponentGenerator::on_codeDirectoryBtn_clicked()
{
    QString val = QFileDialog::getExistingDirectory(this,
        tr("Pick a code directory"));
    ui->codeDirectoryEdit->setText(val);
}

void ComponentGenerator::on_dshostHeadersBtn_clicked()
{
    QString val = QFileDialog::getExistingDirectory(this,
        tr("Pick a dshost headers directory"));
    ui->dshostHeadersEdit->setText(val);
}
