// edit_component_dialog.cpp

#include "edit_component_dialog.hpp"
#include "IDEProjectProperties.hpp"
#include "component_parser.hpp"
// #include "config.h"
#include "interface_store_model.hpp"
#include "interfaces_select_dialog.hpp"
#include "proxy_interface_model.hpp"
#include "manager/IDEProjectCreator.hpp"
#include "ui_edit_component_dialog.h"

#include <QDir>
#include <QDirIterator>
#include <QMessageBox>
#include <QRegExpValidator>
#include <QTextStream>
#include <QUuid>
#include <iostream>

#define XML_DIR "./interfaces"

edit_component_dialog::edit_component_dialog(const QDir &path_to_generate, QWidget* parent)
: QDialog {parent}
, ui_(new Ui::edit_component_dialog)
, select_dialog_ {nullptr}
, path_ {path_to_generate}
{
    ui_->setupUi(this);
    ui_->guid_view->setReadOnly(true);
    ui_->Identifier_edit->setValidator(new ::QRegExpValidator {QRegExp {"[A-Za-z0-9_-]+"}, this});

    connect(ui_->okBtn, &QPushButton::clicked, this, &edit_component_dialog::save_parameters);
    connect(ui_->cancelBtn, &QPushButton::clicked, this, &edit_component_dialog::reject);

    // directory, when all description (xml) files
    ::QDir path_to_description_files {XML_DIR};
    component_parser parser;

    // load all xml files in model
    ::QDirIterator iterator {
    path_to_description_files.absolutePath(), QStringList() << "*.xml", QDir::Files, QDirIterator::Subdirectories
    };
    while (iterator.hasNext())
    {
        try
        {
            all_interfaces_.add_interface(parser.read_description_file(iterator.next()));
        }
        catch (std::exception &e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    // model which intake all interfaces
    auto all_interfaces_model = new interface_store_model {all_interfaces_.inheritance_interfaces_, this};

    // this proxy model filter interfaces by input string
    auto filter_proxy_model = new proxy_interface_model {this};
    filter_proxy_model->setSourceModel(all_interfaces_model);
    filter_proxy_model->setRecursiveFilteringEnabled(false);

    select_dialog_ = new interfaces_select_dialog(filter_proxy_model, this);

    ui_->interfaces_view->header()->setSectionResizeMode(QHeaderView::Stretch);
    ui_->interfaces_view->setRootIsDecorated(false);

    // model which will be used for create project
    auto output_component_model = new interface_store_model {creator_.inheritance_interfaces_, this};
    ui_->interfaces_view->setModel(output_component_model);
    // when data is set, first column automatically resize to context
    connect(output_component_model, &::QAbstractItemModel::dataChanged, ui_->interfaces_view, [=]()
    {
        ui_->interfaces_view->resizeColumnToContents(0);
    });

    connect(ui_->addBtn, &::QPushButton::clicked, select_dialog_, &::QDialog::exec);
    connect(select_dialog_, &interfaces_select_dialog::selected_index, this, [=](const QModelIndex &index)
    {
        if (!index.isValid())
        {
            return;
        }
        auto curdata = index.data(Qt::EditRole);
        if (curdata.isValid())
        {
            output_component_model->insertRow(output_component_model->rowCount());
            auto out_index = output_component_model->index(output_component_model->rowCount() - 1, 0);
            output_component_model->setData(out_index, curdata);
        }
    });
    connect(ui_->deleteBtn, &::QPushButton::clicked, this, [=]()
    {
        auto cur_index = ui_->interfaces_view->currentIndex();
        if (cur_index.isValid() && !cur_index.parent().isValid())
        {
            output_component_model->removeRow(cur_index.row());
        }
    });
}

edit_component_dialog::~edit_component_dialog()
{
    delete ui_;
}

void edit_component_dialog::save_parameters()
{
    dscontrol::IDEProjectProperties prop;
    prop.exec();
    if (prop.result() == QDialog::Rejected)
    {
        return;
    }
    QStringList files;
    try
    {
        QString class_name = ui_->Identifier_edit->text();
        if (class_name.isEmpty())
        {
            throw std::runtime_error {"identifier of component not set"};
        }
        int type = prop.IDEType();
        QString name = prop.Name();
        QString path = prop.Path();

        files = creator_.generate_project(class_name, path);

        QStringList compile;
        QStringList include;
        for (auto file : files)
        {
            QFileInfo info {file};
            if (info.suffix() == "cpp")
            {
                compile.append(file);
            }
            if (info.suffix() == "h" || info.suffix() == "hpp")
            {
                include.append(file);
            }
        }
        QString toLaunch;
        switch (type)
        {
            case dscontrol::IDEProjectCreator::VisualStudio:
            {
                std::map<QUuid, QString> proj;
                proj.insert(dscontrol::IDEProjectCreator::createProject(name, compile, include, path, type));
                toLaunch = dscontrol::IDEProjectCreator::createVSSolution(proj, path, name);
                break;
            }
            case dscontrol::IDEProjectCreator::QtCreator:
                toLaunch = dscontrol::IDEProjectCreator::createProject(name, compile, include, path, type).second;
                break;
            default:
                break;
        }
        dscontrol::IDEProjectCreator::launchProject(QStringList() << toLaunch, type);
    }
    catch (std::exception &except)
    {
        for (const auto &i : files)
        {
            ::QFile {i}.remove();
        }
        ::QMessageBox error_message {};
        error_message.setText(except.what());
        error_message.exec();
        return;
    }
    for (auto &i : files)
    {
        if (i.contains("CMakeLists.txt"))
        {
            build_project_file_ = i;
            break;
        }
    }
    this->accepted();
    this->reject();
}

QString edit_component_dialog::create_component()
{
    exec();
    return build_project_file_;
}

void edit_component_dialog::changeEvent(QEvent* event)
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
