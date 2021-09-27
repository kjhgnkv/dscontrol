#include "ArrowUISettings.h"

#include "common/globals.h"
#include "ui_ArrowUISettings.h"

#include <QColorDialog>

ArrowUiSettings::ArrowUiSettings(QWidget* parent)
: QGroupBox(parent)
, qss_ {}
, currentColor_ {}
, ui_(new Ui::ArrowUISettings)
{
    ui_->setupUi(this);
    qss_ = R"(
              QPushButton {
              border: none;
              background-color: curColor;
              }
             )";

    connect(ui_->colorButton, &QPushButton::clicked, this, &ArrowUiSettings::colorButtonClicked);
}

ArrowUiSettings::~ArrowUiSettings()
{
    delete ui_;
}

const QColor ArrowUiSettings::selectColor(const QColor &initColor)
{
    QColorDialog dialog(initColor);
    dialog.setStyleSheet(_DEF_STYLESHEET);
    return dialog.exec() == QDialog::Accepted ? dialog.currentColor() : initColor;
}

void ArrowUiSettings::setColor(const QColor &color)
{
    currentColor_ = color;
    ui_->colorButton->setIcon({});
    QString qss = qss_;
    ui_->colorButton->setStyleSheet(qss.replace("curColor", color.name()));
    ui_->colorButton->update();
}

void ArrowUiSettings::setValues(int width, int direction, const QColor &color)
{
    ui_->widthSpinBox->setValue(width);
    ui_->directionComboBox->setCurrentIndex(direction);
    setColor(color);
}

void ArrowUiSettings::colorButtonClicked()
{
    setColor(selectColor(currentColor_));
}

int ArrowUiSettings::width()
{
    return ui_->widthSpinBox->value();
}

int ArrowUiSettings::direction()
{
    return ui_->directionComboBox->currentIndex();
}

QColor ArrowUiSettings::color()
{
    return currentColor_;
}

