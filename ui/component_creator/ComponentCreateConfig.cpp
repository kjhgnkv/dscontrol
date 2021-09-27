#include "ComponentCreateConfig.h"
#include "ui_ComponentCreateConfig.h"

#include <QFileDialog>
#include <Poco/File.h>
#include <Poco/Path.h>
using namespace dscontrol;
//this modeule will load CodeFrameConfig.xml from application path, and
//will copy macchina.cmake and build.cmake from application path during
//generation code frame.
ComponentCreateConfig::ComponentCreateConfig(QWidget *parent) :
    QDialog(parent),
    ui_(new Ui::ComponentCreateConfig)
{
    ui_->setupUi(this);
    _pGroup=new QButtonGroup(this);
    setVisibleForDerived();
    _pGroup->addButton(ui_->radioButton_generalType, DerivedType::general);
    _pGroup->addButton(ui_->radioButton_protocolType, DerivedType::protocol);
    _pGroup->addButton(ui_->radioButton_transportType, DerivedType::transport);
    ui_->radioButton_generalType->setChecked(true);
    //set up recipe manager
    _recipeManager = new RecipeManager();
    _recipeManager->init();
    //try to load xml, and we will show error if there is any exception
    QString  appPath=QCoreApplication::applicationDirPath();
    std::string xmlPath=appPath.toStdString();
    if (Poco::Path::separator() != *xmlPath.rbegin())
    {
        xmlPath += Poco::Path::separator();
    }
    xmlPath += "CodeFrameConfig.xml";
    Poco::File file(xmlPath);
    if(!file.exists())
        ui_->label_status->setText("CodeFrameConfig.xml does not exist.");
    else
    {
        try{
        _recipeManager->loadFromXML(xmlPath);
        }
        catch(std::runtime_error& e)
        {
            QString error="load config error:";
            error+=e.what();
            ui_->label_status->setText(error);
            ui_->buttonOk->setEnabled(false);
        }
    }
}

ComponentCreateConfig::~ComponentCreateConfig()
{
    delete ui_;
}

void ComponentCreateConfig::on_buttonOk_clicked()
{
    //check config information user input
    if(!checkCommentConfig())
    {
        ui_->label_status->setText("Information is not correct.");
        return;
    }
    if(static_cast<ComponentType>(ui_->comboBoxType->currentIndex())
            ==ComponentType::Derived)
    {
        if(!checkDerivedConfig())
        {
            ui_->label_status->setText("Information is not correct.");
            return;
        }
    }
    //try to generate the code frame, and show result
    try{
        QString xml=getConfig();
        _recipeManager->generate(getComponentTypeName().toStdString(), xml.toStdString());
        ui_->label_status->setText("General successfully.");
    }
    catch(std::exception& e){
        QString error="General error:";
        error+=e.what();
        ui_->label_status->setText(error);
    }
}

void ComponentCreateConfig::on_buttonCancel_clicked()
{
    reject();
}
void ComponentCreateConfig::setVisibleForDerived(bool visable)
{
    ui_->label_baseClass->setVisible(visable);
    ui_->label_baseClassName->setVisible(visable);
    ui_->label_baseClassHeader->setVisible(visable);
    ui_->label_baseFactory->setVisible(visable);
    ui_->label_baseFactoryName->setVisible(visable);
    ui_->label_baseFactoryHeader->setVisible(visable);
    ui_->label_drivedType->setVisible(visable);
    ui_->lineEdit_baseClassName->setVisible(visable);
    ui_->lineEdit_baseClassHeader->setVisible(visable);
    ui_->lineEdit_baseFactoryName->setVisible(visable);
    ui_->lineEdit_baseFactoryHeader->setVisible(visable);
    ui_->pushButton_baseHeader->setVisible(visable);
    ui_->pushButton_baseFactoryHeader->setVisible(visable);
    ui_->radioButton_generalType->setVisible(visable);
    ui_->radioButton_protocolType->setVisible(visable);
    ui_->radioButton_transportType->setVisible(visable);
}
void ComponentCreateConfig::on_comboBoxType_currentIndexChanged(int index)
{
    switch(static_cast<ComponentType>(index))
    {
    case ComponentType::Blank:
    case ComponentType::General:
    case ComponentType::Transport:
    case ComponentType::Protocol:
        setVisibleForDerived();
        break;
    case ComponentType::Derived:
        setVisibleForDerived(true);
        break;
    default:
        break;
    }

}

QString ComponentCreateConfig::getConfig()
{
    int checkedType=_pGroup->checkedId();
    QString derivedType="general";
    switch(checkedType){
    case DerivedType::general:
        derivedType="general";
        break;
    case DerivedType::protocol:
        derivedType="protocol";
        break;
    case DerivedType::transport:
        derivedType="transport";
        break;
    }

    QString  appPath=QCoreApplication::applicationDirPath();
    std::string cmakePath=appPath.toStdString();
    if (Poco::Path::separator() != *cmakePath.rbegin())
    {
        cmakePath += Poco::Path::separator();
    }
    std::string buildCmakeFile= cmakePath + "build.cmake";
    std::string macchinaCmakeFile= cmakePath + "macchina.cmake";
    QString xml= "<?xml version=\"1.0\" encoding=\"utf-8\"?> \n";
    xml+="<RecipeTemplate case=\""+derivedType +"\" destination=\""+ui_->editDirectory->text()+"\" type=\""+ui_->comboBoxType->currentText() + "\"";
    xml+=" buildcmake=\"" + QString(buildCmakeFile.c_str()) +"\" macchinacmake=\""+QString(macchinaCmakeFile.c_str())+"\"> \n";
    xml+="\t<item symbol=\"[#name]\" value=\""+ui_->editClassName->text() +"\"/> \n";
    xml+="\t<item symbol=\"[#BundleName]\" value=\"" +ui_->editBundleName->text()+ "\"/> \n";
    xml+="\t<item symbol=\"[#BundleDescriber]\" value = \""+ ui_->editDescription->text()+"\"/> \n";
    xml+="\t<item symbol=\"[#BundleVersion]\" value=\""+ui_->editVersion->text()+"\"/> \n";
    xml+="\t<item symbol=\"[#vendor]\" value=\""+ui_->editVendor->text()+"\"/> \n";
    xml+="\t<item symbol=\"[#CopyRight]\" value=\""+ui_->editCopyRight->text()+"\"/> \n";
    xml+="\t<item symbol=\"[#LibraryName]\" value=\""+ui_->editLibraryName->text()+"\"/> \n";
    xml+="\t<item symbol=\"[#LazyStart]\" value=\"true\"/> \n\
    <item symbol=\"[#RunLevel]\" value=\"44\"/> \n";
    xml+="\t<item symbol=\"[#OutputDir]\" value=\""+ui_->editOutputDir->text()+"\"/> \n";
    xml+="\t<item symbol=\"[#DSHostInclude]\" value=\""+ui_->editIncludePath->text().replace(";", " ")+"\"/> \n";
    xml+="\t<item symbol=\"[#LibrariesDSUtils]\" value=\""+ui_->editLibaryPath->text().replace(";", " ")+"\"/> \n";
    xml+="\t<item symbol=\"[#BaseClass]\" value=\""+ui_->lineEdit_baseClassName->text()+"\"/>\n";
    xml+="\t<item symbol=\"[#BaseClassHeaderFile]\" value=\""+ui_->lineEdit_baseClassHeader->text()+"\"/>\n";
    xml+="\t<item symbol=\"[#BaseClassFactory]\" value=\""+ui_->lineEdit_baseFactoryName->text()+"\"/>\n";
    xml+="\t<item symbol=\"[#BaseClassFactoryHeaderFile]\" value=\""+ui_->lineEdit_baseFactoryHeader->text()+"\"/>\n";
    xml+="\t<item symbol=\"[#BaseClassInclude]\" value=\""+ui_->lineEdit_baseFactoryHeader->text()+"\"/>\n";
    xml+="\t<item symbol=\"[#BaseLibary]\" value=\""+ui_->editLibaries->text().replace(";", " ")+"\"/>\n";
    xml+="</RecipeTemplate>\n" ;
    return xml;
}

QString ComponentCreateConfig::getComponentTypeName()
{
    return ui_->comboBoxType->currentText();
}
void ComponentCreateConfig::on_buttonDirectory_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(
           this, tr("Choose Directory"), ui_->editDirectory->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(dir.isEmpty())
        return;
    ui_->editDirectory->setText(dir);
}

void dscontrol::ComponentCreateConfig::on_buttonAddInclude_clicked()
{
    QString selectDir=ui_->editIncludePath->text();
    QStringList sections=selectDir.split(";");
    QString startPosition="";
    if(sections.size() > 0)
        startPosition=sections.at(sections.size()-1);
    QString dir = QFileDialog::getExistingDirectory(
           this, tr("Choose Directory"), startPosition, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if(!dir.isEmpty() && !findStringBySeparator(selectDir, dir, PATH_SEPARATOR))
    {
        if(!selectDir.isEmpty())
            selectDir+=PATH_SEPARATOR+dir;
        else
            selectDir=dir;
        ui_->editIncludePath->setText(selectDir);
    }
}

void dscontrol::ComponentCreateConfig::on_buttonAddLibary_clicked()
{
    QString selectFiles=ui_->editLibaries->text();
    QStringList sections=selectFiles.split(";");

    QString lib = QFileDialog::getOpenFileName(this,
              tr("Choose File"), "", tr("Lib Files(*.lib *.so)"));
    //add lib name to list
    if(!lib.isEmpty())
    {
        Poco::Path path(lib.toStdString());
        QString filename(path.getFileName().c_str());
        if(!selectFiles.isEmpty())
            selectFiles+=PATH_SEPARATOR+filename;
        else
            selectFiles=filename;
        ui_->editLibaries->setText(selectFiles);
        //add lib path to lib directories
        QString dir=lib;
        dir.replace(filename, "");
        QString libDir=ui_->editLibaryPath->text();
        if(!findStringBySeparator(libDir, dir, PATH_SEPARATOR))
        {
            if(!libDir.isEmpty())
                libDir+=";"+dir;
            else
                libDir=dir;
            ui_->editLibaryPath->setText(libDir);
        }
    }
}

void dscontrol::ComponentCreateConfig::on_pushAddLibDir_clicked()
{
    QString selectDir=ui_->editLibaryPath->text();
    QStringList sections=selectDir.split(PATH_SEPARATOR);
    QString startPosition="";
    if(sections.size() > 0)
        startPosition=sections.at(sections.size()-1);
    QString dir = QFileDialog::getExistingDirectory(
           this, tr("Choose Directory"), startPosition, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if(!dir.isEmpty())
    {
        if(!selectDir.isEmpty())
            selectDir+=PATH_SEPARATOR+dir;
        else
            selectDir=dir;
        ui_->editLibaryPath->setText(selectDir);
    }
}
//check config information
//return true if no problem, or return false and focus on the
//first control that has issue.
bool dscontrol::ComponentCreateConfig::checkCommentConfig()
{
    if(ui_->editDirectory->text().isEmpty())
    {
        ui_->editDirectory->setFocus();
        return false;
    }
    else
    {
        std::string dirPath=ui_->editDirectory->text().toStdString();

        if (Poco::Path::separator() != *dirPath.rbegin())
        {
            dirPath += Poco::Path::separator();
        }
        ui_->editDirectory->setText(QString(dirPath.c_str()));
        Poco::Path path(dirPath);
        if(!path.isDirectory())
        {
            ui_->editDirectory->setFocus();
            return false;
        }
    }
    if(ui_->editClassName->text().isEmpty())
    {
        ui_->editClassName->setFocus();
        return false;
    }
    if(ui_->editBundleName->text().isEmpty())
    {
        ui_->editBundleName->setFocus();
        return false;
    }
    if(ui_->editLibraryName->text().isEmpty())
    {
        ui_->editLibraryName->setFocus();
        return false;
    }
    if(ui_->editOutputDir->text().isEmpty())
    {
        ui_->editOutputDir->setFocus();
        return false;
    }
    return true;
}
//check config information for Derived component
//return true if no problem, or return false and focus on the
//first control that has issue.
bool dscontrol::ComponentCreateConfig::checkDerivedConfig()
{
    if(ui_->lineEdit_baseClassName->text().isEmpty())
    {
        ui_->lineEdit_baseClassName->setFocus();
        return false;
    }
    if(ui_->lineEdit_baseClassHeader->text().isEmpty())
    {
        ui_->lineEdit_baseClassHeader->setFocus();
        return false;
    }
    if(ui_->lineEdit_baseFactoryName->text().isEmpty())
    {
        ui_->lineEdit_baseFactoryName->setFocus();
        return false;
    }
    if(ui_->lineEdit_baseFactoryHeader->text().isEmpty())
    {
        ui_->lineEdit_baseFactoryHeader->setFocus();
        return false;
    }
    return true;
}

void dscontrol::ComponentCreateConfig::on_pushButton_baseHeader_clicked()
{
    QString file = QFileDialog::getOpenFileName(this,
              tr("Choose Header File"), "", tr("Header Files(*.h *.hpp)"));

    if(!file.isEmpty())
    {
        ui_->lineEdit_baseClassHeader->setText(file);
    }
}

void dscontrol::ComponentCreateConfig::on_pushButton_baseFactoryHeader_clicked()
{
    QString file = QFileDialog::getOpenFileName(this,
              tr("Choose Header File"), "", tr("Header Files(*.h *.hpp)"));

    if(!file.isEmpty())
    {
        ui_->lineEdit_baseFactoryHeader->setText(file);
    }
}
//this function will split source string into serveral substring by given Separator
//then try to find a value in these sub-strings
//pararmter:str, the source string
//value the string you want to find
//separator is the Separator
//if find return true, or return false;
bool dscontrol::ComponentCreateConfig::findStringBySeparator(QString str, QString value, QString separator)
{
    bool find=false;
    QStringList sections=str.split(separator);
    for(QStringList::iterator it=sections.begin(); it!=sections.end();it++)
    {
        if((*it).compare(value)==0)
        {
            find=true;
            break;
        }
    }
    return find;
}
