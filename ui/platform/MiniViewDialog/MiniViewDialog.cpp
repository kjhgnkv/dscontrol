#include "MiniViewDialog.hpp"

#include "nodeeditor/scenes/FlowScene.hpp"
#include "common/globals.h"
#include "ui_MiniViewDialog.h"
#include "nodeeditor/views/FlowView.hpp"
#include <QCloseEvent>
#include <QDebug>
#include <QPushButton>

namespace dscontrol
{
MiniViewDialog::MiniViewDialog(QtNodes::FlowView* parentView, QtNodes::FlowScene* parentScene, QWidget* parent)
: QDialog(parent)
, ui(new Ui::MiniViewDialog)
{
    qDebug() << "MiniViewDialog: constructor";
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);

    if (parentScene && parentView)
    {
        parentView_ = parentView;
        scene_ = parentScene;
        ui->view_->setFlowScene(scene_);
        ui->view_->setOriginalView(parentView_);
    }

    ui->slider_->setMinimum(MIN_SCALE / 10);
    ui->slider_->setMaximum(MAX_SCALE / 10);
    ui->slider_->setSingleStep(1);
    ui->slider_->setPageStep(1);
    ui->slider_->setTickPosition(QSlider::TicksBothSides);
    ui->slider_->setValue(qRound(parentView->getCurrentScale() / 10.0));

    makeConnections();
}

MiniViewDialog::~MiniViewDialog()
{
    qDebug() << "MiniViewDialog: destructor";
    delete ui;
}

void MiniViewDialog::setNewContent(QtNodes::FlowView* parentView, QtNodes::FlowScene* parentScene)
{
    if (!parentScene || !parentView)
    {
        qWarning() << "MiniViewDialog: nullptr scene or view;";
        return;
    }

    scene_ = parentScene;
    parentView_ = parentView;

    ui->view_->setFlowScene(scene_);
    ui->view_->setOriginalView(parentView_);
    ui->slider_->setValue(qRound(parentView->getCurrentScale() / 10.0));

    makeDisconnection();
    makeConnections();
}

void MiniViewDialog::closeEvent(QCloseEvent* event)
{
    emit closed();
    event->accept();
}

void MiniViewDialog::keyPressEvent(QKeyEvent* event)
{
    if (nullptr != event && Qt::Key_Escape == event->key())
    {
        this->close();
    }
}

void MiniViewDialog::makeConnections()
{
    if (!parentView_ || !scene_)
    {
        qWarning() << "MiniViewDialog: nullptr scene or view;";
        return;
    }

    connect(parentView_, &QtNodes::FlowView::scaleChanged, this, &MiniViewDialog::setValue, Qt::UniqueConnection);

    connect(ui->slider_, &QSlider::valueChanged, this, &MiniViewDialog::scaleOriginView, Qt::UniqueConnection);

    connect(ui->btnZoomIn_, &QPushButton::clicked, parentView_, &QtNodes::FlowView::scaleUp, Qt::UniqueConnection);
    connect(ui->btnZoomOut_, &QPushButton::clicked, parentView_, &QtNodes::FlowView::scaleDown, Qt::UniqueConnection);

    connect(ui->btnZoomIn_2, &QPushButton::clicked, parentView_, &QtNodes::FlowView::scaleUp, Qt::UniqueConnection);
    connect(ui->btnZoomOut_2, &QPushButton::clicked, parentView_, &QtNodes::FlowView::scaleDown, Qt::UniqueConnection);
}

void MiniViewDialog::makeDisconnection()
{
    if (!parentView_ || !scene_)
    {
        qWarning() << "MiniViewDialog: nullptr scene or view;";
        return;
    }

    parentView_->disconnect(nullptr, this, nullptr);
    ui->btnZoomIn_->disconnect(nullptr, parentView_, nullptr);
    ui->btnZoomOut_->disconnect(nullptr, parentView_, nullptr);
    ui->btnZoomIn_2->disconnect(nullptr, parentView_, nullptr);
    ui->btnZoomOut_2->disconnect(nullptr, parentView_, nullptr);
    ui->slider_->disconnect(nullptr, this, nullptr);
}

void dscontrol::MiniViewDialog::setValue(const int &value)
{
    ui->slider_->setValue(value / 10);
}

void dscontrol::MiniViewDialog::scaleOriginView(const int &value)
{
    if (parentView_)
        parentView_->setScale(value * 10);
}
} // namespace dscontrol
