#include "ToolBarWidget.h"

#include "nodes/UniversalGraphicsItemImpl.h"
#include "common/globals.h"
#include "ui_ToolBarWidget.h"
#include "ToolBarWidgetProperties.h"

#include <QApplication>
#include <QColorDialog>
#include <QFontDialog>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QSpinBox>

using namespace QtNodes;

ToolBarWidget::ToolBarWidget(QWidget* parent)
: QWidget {parent}
, ui_ {new Ui::ToolBarWidget}
, properties_ {new ToolBarWidget::Properties}
{
    ui_->setupUi(this);

    connect(ui_->fontBtn, &QPushButton::clicked, this, &ToolBarWidget::fontButtonClicked);
    connect(ui_->colorBtn, &QPushButton::clicked, this, &ToolBarWidget::colorButtonClicked);
    connect(ui_->backColorBtn, &QPushButton::clicked, this, &ToolBarWidget::backColorButtonClicked);
    connect(ui_->borderColorBtn, &QPushButton::clicked, this, &ToolBarWidget::borderColorButtonClicked);
    connect(ui_->spinBox, qOverload<qint32>(&QSpinBox::valueChanged), this, &ToolBarWidget::spinBoxValueChanged);
    connect(ui_->resetBtn, &QPushButton::clicked, this, &ToolBarWidget::reseted);
}

ToolBarWidget::~ToolBarWidget()
{
    delete ui_;
}

void ToolBarWidget::setProperties(const QFont &font, const QColor &color, const QColor &backColor,
                                  const QColor &borderColor, qint32 borderWidth)
{
    properties_->font = font;
    properties_->color = color;
    properties_->backColor = backColor;
    properties_->borderColor = borderColor;
    properties_->borderWidth = borderWidth;
    ui_->spinBox->setValue(borderWidth);
    updateStyleSheets();
}

void ToolBarWidget::setProperties(const ToolBarWidget::Properties &properties)
{
    setProperties(properties.font, properties.color, properties.backColor, properties.borderColor,
                  properties_->borderWidth);
}

void ToolBarWidget::updateStyleSheets()
{
    ui_->fontBtn->setIcon({});
    ui_->colorBtn->setIcon({});
    ui_->backColorBtn->setIcon({});
    ui_->borderColorBtn->setIcon({});
    ui_->resetBtn->setIcon({});

    QString qss = R"(
                  QPushButton {
                  border: none;
                  background-color: curColor;
                  image: url(:/toolbar/iconName.png);
                  }
                  QPushButton:hover {
                  image: url(:/toolbar/iconNameHovered.png);
                  }
                  QPushButton:disabled {
                  image: url(:/toolbar/iconNameDisabled.png);
                  }
                  )";

    QString qss_ = qss;
    ui_->fontBtn->setStyleSheet(qss_.replace("curColor", "transparent").replace("iconName", "font"));

    qss_ = qss;
    ui_->colorBtn->setStyleSheet(qss_.replace("curColor", properties_->color.name()).replace("iconName", "color"));

    qss_ = qss;
    ui_->backColorBtn->setStyleSheet(
    qss_.replace("curColor", properties_->backColor.name()).replace("iconName", "backColor"));

    qss_ = qss;
    ui_->borderColorBtn->setStyleSheet(
    qss_.replace("curColor", properties_->borderColor.name()).replace("iconName", "borderColor"));

    qss_ = qss;
    ui_->resetBtn->setStyleSheet(qss_.replace("curColor", "transparent").replace("iconName", "reset"));
}

void ToolBarWidget::setMode(Mode mode)
{
    if (mode == Item)
    {
        ui_->fontBtn->setEnabled(true);
        ui_->colorBtn->setEnabled(true);
        ui_->backColorBtn->setEnabled(true);
        ui_->borderColorBtn->setEnabled(true);
        ui_->spinBox->setEnabled(false);
        ui_->resetBtn->setEnabled(true);
    }
    else if (mode == Line)
    {
        ui_->fontBtn->setEnabled(false);
        ui_->colorBtn->setEnabled(false);
        ui_->backColorBtn->setEnabled(true);
        ui_->borderColorBtn->setEnabled(false);
        ui_->spinBox->setEnabled(true);
        ui_->resetBtn->setEnabled(true);
    }
    else if (mode == Text)
    {
        ui_->fontBtn->setEnabled(true);
        ui_->colorBtn->setEnabled(true);
        ui_->backColorBtn->setEnabled(true);
        ui_->borderColorBtn->setEnabled(true);
        ui_->spinBox->setEnabled(true);
        ui_->resetBtn->setEnabled(true);
    }
    else if (mode == Frame)
    {
        ui_->fontBtn->setEnabled(true);
        ui_->colorBtn->setEnabled(true);
        ui_->backColorBtn->setEnabled(false);
        ui_->borderColorBtn->setEnabled(true);
        ui_->spinBox->setEnabled(true);
        ui_->resetBtn->setEnabled(true);
    }
}

const QColor ToolBarWidget::selectColor(const QColor &initColor)
{
    QColorDialog dialog(initColor);
    dialog.setStyleSheet(_DEF_STYLESHEET);
    return dialog.exec() == QDialog::Accepted ? dialog.currentColor() : initColor;
}

void ToolBarWidget::fontButtonClicked()
{
    emit interactionHasStarted();
    QFontDialog dlg;
    dlg.setStyleSheet(_DEF_STYLESHEET);
    dlg.exec();
    if (dlg.result() != QDialog::Rejected)
    {
        emit itemPropertyChanged(Item::GraphicsItemProperties::Font, dlg.currentFont());
        properties_->font = dlg.currentFont();
    }

    emit interactionHasFinished();
}

void ToolBarWidget::colorButtonClicked()
{
    colorButtonClickedHelper(ColorType::Color);
}

void ToolBarWidget::backColorButtonClicked()
{
    colorButtonClickedHelper(ColorType::BackColor);
}

void ToolBarWidget::borderColorButtonClicked()
{
    colorButtonClickedHelper(ColorType::BorderColor);
}

void ToolBarWidget::spinBoxValueChanged(qint32 value)
{
    emit itemPropertyChanged(Item::GraphicsItemProperties::BorderWidth, value);
}

void ToolBarWidget::mousePressEvent(QMouseEvent* event)
{
    event->accept();
}

void ToolBarWidget::colorButtonClickedHelper(ToolBarWidget::ColorType colorType)
{
    emit interactionHasStarted();

    QColor* initColor = nullptr;
    auto initProperty = Item::GraphicsItemProperties::None;

    switch (colorType)
    {
        case ColorType::Color:
        {
            initColor = &properties_->color;
            initProperty = Item::GraphicsItemProperties::Color;
            break;
        }
        case ColorType::BackColor:
        {
            initColor = &properties_->backColor;
            initProperty = Item::GraphicsItemProperties::BackColor;
            break;
        }
        case ColorType::BorderColor:
        {
            initColor = &properties_->borderColor;
            initProperty = Item::GraphicsItemProperties::BorderColor;
            break;
        }
        default:
        {
            return;
        }
    }

    const QColor &newColor = selectColor(*initColor);
    emit itemPropertyChanged(initProperty, newColor);
    *initColor = newColor;

    updateStyleSheets();
    emit interactionHasFinished();
}
