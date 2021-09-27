#include "UniversalGraphicsWidget.h"

#include "nodes/UniversalGraphicsItemImpl.h"
#include "scenes/FlowScene.hpp"
#include "ui_UniversalGraphicsWidget.h"
#include <QDebug>
#include <QLabel>
#include <QSvgWidget>

namespace QtNodes
{
UniversalGraphicsWidget::UniversalGraphicsWidget(QtNodes::FlowScene* scene, QWidget* parent)
: QWidget {parent}
, ui_ {new Ui::UniversalGraphicsWidget}
{
    scene_ = scene;
    ui_->setupUi(this);
    setMaximumSize(32670, 32670);

    ui_->pictureLabel->setScaledContents(true);
}

UniversalGraphicsWidget::~UniversalGraphicsWidget()
{
    delete ui_;
}

FlowView* UniversalGraphicsWidget::getFlowView()
{
    return ui_->graphicsView;
}

void UniversalGraphicsWidget::setScene(QtNodes::FlowScene* scene)
{
    if (scene)
        ui_->graphicsView->setFlowScene(scene);
}

void UniversalGraphicsWidget::setGeometry(QRect rect)
{
    QWidget::setGeometry(rect);

    setFixedSize(rect.width(), rect.height());
}

void UniversalGraphicsWidget::setBackColor(const QColor &color)
{
    ui_->graphicsView->setBackgroundBrush(color);
    ui_->pictureLabel->setBackColor(color);
}

void UniversalGraphicsWidget::setPicture(const QPixmap &pixmap)
{
    ui_->pictureLabel->setPicture(pixmap);
}

void UniversalGraphicsWidget::setViewMode(int mode)
{
    switch (mode)
    {
        case UniversalGraphicsItemImpl::Scene:
        case UniversalGraphicsItemImpl::Component:
        {
            ui_->stackedWidget->setCurrentWidget(ui_->scenePage);
            break;
        }
        case UniversalGraphicsItemImpl::Picture:
        case UniversalGraphicsItemImpl::Hidden:
        {
            ui_->stackedWidget->setCurrentWidget(ui_->picturePage);
            break;
        }
        default:
            break;
    }
}

QtNodes::FlowScene* UniversalGraphicsWidget::nestedScene()
{
    return scene_;
}

QtNodes::FlowView* UniversalGraphicsWidget::nestedView()
{
    return ui_->graphicsView;
}

void UniversalGraphicsWidget::setPictureBackColor(const QColor &color)
{
    ui_->pictureLabel->setBackColor(color);
}

void UniversalGraphicsWidget::moveTitle()
{
    resizeEvent(nullptr);
}

void UniversalGraphicsWidget::contextMenuEvent(QContextMenuEvent* event)
{
    QWidget::contextMenuEvent(event);
}
} // namespace QtNodes
