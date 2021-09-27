#include "SceneSettings.h"

#include "common/globals.h"
#include "ui_SceneSettings.h"

#include <QColorDialog>

SceneSettings::SceneSettings(QWidget* parent)
: QGroupBox(parent)
, step_ {STEP}
, qss_ {}
, backgroundColor_ {}
, ui_(new Ui::SceneSettings)
{
    ui_->setupUi(this);
    qss_ = R"(
              QPushButton {
              border: none;
              background-color: curColor;
              }
             )";

    ui_->minimumSpinBox->setMinimum(MIN_SCALE);
    ui_->minimumSpinBox->setMaximum(MAX_SCALE);
    ui_->maximumSpinBox->setMinimum(MIN_SCALE);
    ui_->maximumSpinBox->setMaximum(MAX_SCALE);
    ui_->hideSpinBox->setMinimum(MIN_SCALE);
    ui_->hideSpinBox->setMaximum(MAX_SCALE);

    connect(ui_->backColorButton, &QPushButton::clicked, this, &SceneSettings::backColorButtonClicked);
    connect(ui_->stepComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SceneSettings::stepChanged);
    connect(ui_->minimumSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int i)
    {
        adjustZoomLimits();
    });
    connect(ui_->maximumSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int i)
    {
        adjustZoomLimits();
    });
}

SceneSettings::~SceneSettings()
{
    delete ui_;
}

const QColor SceneSettings::selectColor(const QColor &initColor)
{
    QColorDialog dialog(initColor);
    dialog.setStyleSheet(_DEF_STYLESHEET);
    return dialog.exec() == QDialog::Accepted ? dialog.currentColor() : initColor;
}

void SceneSettings::setColor(const QColor &color)
{
    backgroundColor_ = color;
    ui_->backColorButton->setIcon({});
    QString qss = qss_;
    ui_->backColorButton->setStyleSheet(qss.replace("curColor", color.name()));
    ui_->backColorButton->update();
}

void SceneSettings::adjustZoomLimits()
{
    ui_->maximumSpinBox->setMinimum(ui_->minimumSpinBox->value() + step_);
    ui_->minimumSpinBox->setMaximum(ui_->maximumSpinBox->value() - step_);
    ui_->hideSpinBox->setMinimum(ui_->minimumSpinBox->value());
    ui_->hideSpinBox->setMaximum(ui_->maximumSpinBox->value());
}

void SceneSettings::setValues(int minZoom, int maxZoom, int step, int hideZoom, const QColor &backColor)
{
    ui_->minimumSpinBox->setValue(minZoom);
    ui_->maximumSpinBox->setValue(maxZoom);
    ui_->hideSpinBox->setValue(hideZoom);
    switch (step)
    {
        case 5:
        {
            ui_->stepComboBox->setCurrentIndex(0);
            break;
        }
        case 10:
        {
            ui_->stepComboBox->setCurrentIndex(1);
            break;
        }
        case 20:
        {
            ui_->stepComboBox->setCurrentIndex(2);
            break;
        }
        default:
        {
            ui_->stepComboBox->setCurrentIndex(1);
            break;
        }
    }
    adjustZoomLimits();
    setColor(backColor);
}

void SceneSettings::backColorButtonClicked()
{
    setColor(selectColor(backgroundColor_));
}

void SceneSettings::stepChanged(int curIndex)
{
    switch (curIndex)
    {
        case 0:
        {
            step_ = 5;
            break;
        }
        case 1:
        {
            step_ = 10;
            break;
        }
        case 2:
        {
            step_ = 20;
            break;
        }
        default:
        {
            step_ = 10;
            break;
        }
    }
    ui_->minimumSpinBox->setSingleStep(step_);
    ui_->maximumSpinBox->setSingleStep(step_);
    ui_->hideSpinBox->setSingleStep(step_);
}

int SceneSettings::minZoom()
{
    return ui_->minimumSpinBox->value();
}

int SceneSettings::maxZoom()
{
    return ui_->maximumSpinBox->value();
}

int SceneSettings::hideTextZoom()
{
    return ui_->hideSpinBox->value();
}

int SceneSettings::step()
{
    return step_;
}

QColor SceneSettings::backgroundColor()
{
    return backgroundColor_;
}

