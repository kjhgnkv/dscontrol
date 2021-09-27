#include "DSSettings.hpp"

#include "ui_DSSettings.h"
#include "manager/DSConfiguration.h"
#include <QFileDialog>
#include <QString>
#include <QListWidget>
#include <QStackedWidget>

using namespace dscontrol;
using namespace QtNodes::Arrows;
using namespace QtNodes::Item;

DSSettings::DSSettings(QWidget* parent)
: QDialog(parent)
, ui_(new Ui::DSSettings)
{
    ui_->setupUi(this);

    connect(ui_->optionsList_, &QListWidget::currentRowChanged, ui_->optionsSettings_,
            &QStackedWidget::setCurrentIndex);

    loadAll();

    #ifdef __linux
    ui_->btnVSPath->setEnabled(false);
    #endif
}

DSSettings::~DSSettings()
{
    delete ui_;
}

void DSSettings::on_btnOk_clicked()
{
    saveAll();
    accept();
    close();
}

void DSSettings::on_btnCancel_clicked()
{
    reject();
    close();
}

void DSSettings::on_btnApply_clicked()
{
    saveAll();
    ui_->btnApply->setEnabled(false);
}

void DSSettings::saveAll()
{
    //code extraction
    DSConfiguration::Instance().setQtPath(ui_->lblQtPath->text());
    DSConfiguration::Instance().setVSPath(ui_->lblVSPath->text());
    //arrows
    //line
    DSConfiguration::Instance().setArrowWidth(ArrowType::Line, ui_->lineUISettings->width());
    DSConfiguration::Instance().setArrowDirection(ArrowType::Line, ui_->lineUISettings->direction());
    DSConfiguration::Instance().setArrowColor(ArrowType::Line, ui_->lineUISettings->color());
    //polyline
    DSConfiguration::Instance().setArrowWidth(ArrowType::PolyLine, ui_->polyLineUISettings->width());
    DSConfiguration::Instance().setArrowDirection(ArrowType::PolyLine, ui_->polyLineUISettings->direction());
    DSConfiguration::Instance().setArrowColor(ArrowType::PolyLine, ui_->polyLineUISettings->color());
    //curved
    DSConfiguration::Instance().setArrowWidth(ArrowType::Curved, ui_->curvedUISettings->width());
    DSConfiguration::Instance().setArrowDirection(ArrowType::Curved, ui_->curvedUISettings->direction());
    DSConfiguration::Instance().setArrowColor(ArrowType::Curved, ui_->curvedUISettings->color());
    //connection
    DSConfiguration::Instance().setArrowWidth(ArrowType::Connection, ui_->connectionUISettings->width());
    DSConfiguration::Instance().setArrowDirection(ArrowType::Connection, ui_->connectionUISettings->direction());
    DSConfiguration::Instance().setArrowColor(ArrowType::Connection, ui_->connectionUISettings->color());
    //lateral horizontal
    DSConfiguration::Instance().setArrowWidth(ArrowType::LateralHorizontal, ui_->hLateralUISettings->width());
    DSConfiguration::Instance().setArrowDirection(ArrowType::LateralHorizontal, ui_->hLateralUISettings->direction());
    DSConfiguration::Instance().setArrowColor(ArrowType::LateralHorizontal, ui_->hLateralUISettings->color());
    //lateral vertical
    DSConfiguration::Instance().setArrowWidth(ArrowType::LateralVertical, ui_->vLateralUISettings->width());
    DSConfiguration::Instance().setArrowDirection(ArrowType::LateralVertical, ui_->vLateralUISettings->direction());
    DSConfiguration::Instance().setArrowColor(ArrowType::LateralVertical, ui_->vLateralUISettings->color());
    //tree horizontal
    DSConfiguration::Instance().setArrowWidth(ArrowType::TreeStyleHorizontal, ui_->hTreeStyleUISettings->width());
    DSConfiguration::Instance().setArrowDirection(ArrowType::TreeStyleHorizontal,
                                                  ui_->hTreeStyleUISettings->direction());
    DSConfiguration::Instance().setArrowColor(ArrowType::TreeStyleHorizontal, ui_->hTreeStyleUISettings->color());
    //tree vertical
    DSConfiguration::Instance().setArrowWidth(ArrowType::TreeStyleVertical, ui_->vTreeStyleUISettings->width());
    DSConfiguration::Instance().setArrowDirection(ArrowType::TreeStyleVertical, ui_->vTreeStyleUISettings->direction());
    DSConfiguration::Instance().setArrowColor(ArrowType::TreeStyleVertical, ui_->vTreeStyleUISettings->color());
    //items
    //system
    DSConfiguration::Instance().setItemBorderWidth(NodeType::SystemNode, ui_->systemUISettings->borderWidth());
    DSConfiguration::Instance().setItemColor(NodeType::SystemNode, ui_->systemUISettings->color());
    DSConfiguration::Instance().setItemBackgroundColor(NodeType::SystemNode, ui_->systemUISettings->backColor());
    DSConfiguration::Instance().setItemBorderColor(NodeType::SystemNode, ui_->systemUISettings->borderColor());
    DSConfiguration::Instance().setItemFont(NodeType::SystemNode, ui_->systemUISettings->font());
    //subsystem
    DSConfiguration::Instance().setItemBorderWidth(NodeType::SubSystemNode, ui_->subSystemGroup->borderWidth());
    DSConfiguration::Instance().setItemColor(NodeType::SubSystemNode, ui_->subSystemGroup->color());
    DSConfiguration::Instance().setItemBackgroundColor(NodeType::SubSystemNode, ui_->subSystemGroup->backColor());
    DSConfiguration::Instance().setItemBorderColor(NodeType::SubSystemNode, ui_->subSystemGroup->borderColor());
    DSConfiguration::Instance().setItemFont(NodeType::SubSystemNode, ui_->subSystemGroup->font());
    //computer
    DSConfiguration::Instance().setItemBorderWidth(NodeType::ComputerNode, ui_->computerUISettings->borderWidth());
    DSConfiguration::Instance().setItemColor(NodeType::ComputerNode, ui_->computerUISettings->color());
    DSConfiguration::Instance().setItemBackgroundColor(NodeType::ComputerNode, ui_->computerUISettings->backColor());
    DSConfiguration::Instance().setItemBorderColor(NodeType::ComputerNode, ui_->computerUISettings->borderColor());
    DSConfiguration::Instance().setItemFont(NodeType::ComputerNode, ui_->computerUISettings->font());
    //application
    DSConfiguration::Instance().setItemBorderWidth(NodeType::ApplicationNode,
                                                   ui_->applicationUISettings->borderWidth());
    DSConfiguration::Instance().setItemColor(NodeType::ApplicationNode, ui_->applicationUISettings->color());
    DSConfiguration::Instance().setItemBackgroundColor(NodeType::ApplicationNode,
                                                       ui_->applicationUISettings->backColor());
    DSConfiguration::Instance().setItemBorderColor(NodeType::ApplicationNode,
                                                   ui_->applicationUISettings->borderColor());
    DSConfiguration::Instance().setItemFont(NodeType::ApplicationNode, ui_->applicationUISettings->font());
    //component
    DSConfiguration::Instance().setItemBorderWidth(NodeType::ComponentNode, ui_->componentUISettings->borderWidth());
    DSConfiguration::Instance().setItemColor(NodeType::ComponentNode, ui_->componentUISettings->color());
    DSConfiguration::Instance().setItemBackgroundColor(NodeType::ComponentNode, ui_->componentUISettings->backColor());
    DSConfiguration::Instance().setItemBorderColor(NodeType::ComponentNode, ui_->componentUISettings->borderColor());
    DSConfiguration::Instance().setItemFont(NodeType::ComponentNode, ui_->componentUISettings->font());
    //repository
    DSConfiguration::Instance().setRepositoryLocalPath(ui_->generalRepositorySettings->localPath());
    DSConfiguration::Instance().setRepositoryRemotePath(ui_->generalRepositorySettings->remotePath());
    DSConfiguration::Instance().setRepositoryFilter(ui_->generalRepositorySettings->filter());
    //scene
    DSConfiguration::Instance().setSceneBackColor(ui_->generalSceneSettings->backgroundColor());
    DSConfiguration::Instance().setSceneMinZoom(ui_->generalSceneSettings->minZoom());
    DSConfiguration::Instance().setSceneMaxZoom(ui_->generalSceneSettings->maxZoom());
    DSConfiguration::Instance().setSceneTextHideZoom(ui_->generalSceneSettings->hideTextZoom());
    DSConfiguration::Instance().setSceneZoomStep(ui_->generalSceneSettings->step());

    DSConfiguration::Instance().save();
}

void DSSettings::loadAll()
{
    //code extraction
    ui_->lblQtPath->setText(DSConfiguration::Instance().qtPath());
    ui_->lblVSPath->setText(DSConfiguration::Instance().vsPath());
    //arrows
    ui_->lineUISettings->setValues(DSConfiguration::Instance().arrowWidth(ArrowType::Line),
                                   DSConfiguration::Instance().arrowDirection(ArrowType::Line),
                                   DSConfiguration::Instance().arrowColor(ArrowType::Line));
    ui_->polyLineUISettings->setValues(DSConfiguration::Instance().arrowWidth(ArrowType::PolyLine),
                                       DSConfiguration::Instance().arrowDirection(ArrowType::PolyLine),
                                       DSConfiguration::Instance().arrowColor(ArrowType::PolyLine));
    ui_->connectionUISettings->setValues(DSConfiguration::Instance().arrowWidth(ArrowType::Connection),
                                         DSConfiguration::Instance().arrowDirection(ArrowType::Connection),
                                         DSConfiguration::Instance().arrowColor(ArrowType::Connection));
    ui_->curvedUISettings->setValues(DSConfiguration::Instance().arrowWidth(ArrowType::Curved),
                                     DSConfiguration::Instance().arrowDirection(ArrowType::Curved),
                                     DSConfiguration::Instance().arrowColor(ArrowType::Curved));
    ui_->hLateralUISettings->setValues(DSConfiguration::Instance().arrowWidth(ArrowType::LateralHorizontal),
                                       DSConfiguration::Instance().arrowDirection(ArrowType::LateralHorizontal),
                                       DSConfiguration::Instance().arrowColor(ArrowType::LateralHorizontal));
    ui_->vLateralUISettings->setValues(DSConfiguration::Instance().arrowWidth(ArrowType::LateralVertical),
                                       DSConfiguration::Instance().arrowDirection(ArrowType::LateralVertical),
                                       DSConfiguration::Instance().arrowColor(ArrowType::LateralVertical));
    ui_->hTreeStyleUISettings->setValues(DSConfiguration::Instance().arrowWidth(ArrowType::TreeStyleHorizontal),
                                         DSConfiguration::Instance().arrowDirection(ArrowType::TreeStyleHorizontal),
                                         DSConfiguration::Instance().arrowColor(ArrowType::TreeStyleHorizontal));
    ui_->vTreeStyleUISettings->setValues(DSConfiguration::Instance().arrowWidth(ArrowType::TreeStyleVertical),
                                         DSConfiguration::Instance().arrowDirection(ArrowType::TreeStyleVertical),
                                         DSConfiguration::Instance().arrowColor(ArrowType::TreeStyleVertical));
    //items
    ui_->systemUISettings->setValues(DSConfiguration::Instance().itemBorderWidth(NodeType::SystemNode),
                                     DSConfiguration::Instance().itemColor(NodeType::SystemNode),
                                     DSConfiguration::Instance().itemBackgroundColor(NodeType::SystemNode),
                                     DSConfiguration::Instance().itemBorderColor(NodeType::SystemNode),
                                     DSConfiguration::Instance().itemFont(NodeType::SystemNode));
    ui_->subSystemGroup->setValues(DSConfiguration::Instance().itemBorderWidth(NodeType::SubSystemNode),
                                   DSConfiguration::Instance().itemColor(NodeType::SubSystemNode),
                                   DSConfiguration::Instance().itemBackgroundColor(NodeType::SubSystemNode),
                                   DSConfiguration::Instance().itemBorderColor(NodeType::SubSystemNode),
                                   DSConfiguration::Instance().itemFont(NodeType::SubSystemNode));
    ui_->computerUISettings->setValues(DSConfiguration::Instance().itemBorderWidth(NodeType::ComputerNode),
                                       DSConfiguration::Instance().itemColor(NodeType::ComputerNode),
                                       DSConfiguration::Instance().itemBackgroundColor(NodeType::ComputerNode),
                                       DSConfiguration::Instance().itemBorderColor(NodeType::ComputerNode),
                                       DSConfiguration::Instance().itemFont(NodeType::ComputerNode));
    ui_->applicationUISettings->setValues(DSConfiguration::Instance().itemBorderWidth(NodeType::ApplicationNode),
                                          DSConfiguration::Instance().itemColor(NodeType::ApplicationNode),
                                          DSConfiguration::Instance().itemBackgroundColor(NodeType::ApplicationNode),
                                          DSConfiguration::Instance().itemBorderColor(NodeType::ApplicationNode),
                                          DSConfiguration::Instance().itemFont(NodeType::ApplicationNode));
    ui_->componentUISettings->setValues(DSConfiguration::Instance().itemBorderWidth(NodeType::ComponentNode),
                                        DSConfiguration::Instance().itemColor(NodeType::ComponentNode),
                                        DSConfiguration::Instance().itemBackgroundColor(NodeType::ComponentNode),
                                        DSConfiguration::Instance().itemBorderColor(NodeType::ComponentNode),
                                        DSConfiguration::Instance().itemFont(NodeType::ComponentNode));
    //repository
    ui_->generalRepositorySettings->setValues(DSConfiguration::Instance().repositoryLocalPath(),
                                              DSConfiguration::Instance().repositoryRemotePath(),
                                              DSConfiguration::Instance().repositoryFilter());
    //scene
    ui_->generalSceneSettings->setValues(DSConfiguration::Instance().sceneMinZoom(),
                                         DSConfiguration::Instance().sceneMaxZoom(),
                                         DSConfiguration::Instance().sceneZoomStep(),
                                         DSConfiguration::Instance().sceneTextHideZoom(),
                                         DSConfiguration::Instance().sceneBackColor());
}

qint32 DSSettings::optionTargetObject(DSSettings::OptionTargetObject row) const
{
    return static_cast<qint32>(row);
}

DSSettings::OptionTargetObject DSSettings::optionTargetObject(qint32 row) const
{
    return static_cast<DSSettings::OptionTargetObject>(row);
}

//TODO create special widget
void DSSettings::on_btnVSPath_clicked()
{
    auto curPath = DSConfiguration::Instance().vsPath();
    QString vsPath = QFileDialog::getOpenFileName(this, tr("Select Visual Studio executable"), curPath,
                                                  tr("Applications (*.exe)"));
    if (!vsPath.isEmpty())
    {
        ui_->lblVSPath->setText(vsPath);
        ui_->btnApply->setEnabled(true);
    }
}

void DSSettings::on_btnQtPath_clicked()
{
    auto curPath = DSConfiguration::Instance().qtPath();
    QString qtPath = QFileDialog::getOpenFileName(this, tr("Select Qt Creator executable"), curPath, "");
    if (!qtPath.isEmpty())
    {
        ui_->lblQtPath->setText(qtPath);
        ui_->btnApply->setEnabled(true);
    }
}
