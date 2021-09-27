#include "ColorButton.hpp"

#include <QColorDialog>
#include <QPalette>

namespace dscontrol
{
ColorButton::ColorButton(QWidget* parent)
: QPushButton {parent}
, currentColor_ {Qt::GlobalColor::black}
{
    connect(this, &QPushButton::clicked, this, &ColorButton::callColorDialog);

    this->setAutoFillBackground(true);
    this->setColor(currentColor_);
}

void ColorButton::setColor(const QColor &color)
{
    currentColor_ = color;
    auto pallete = this->palette();
    pallete.setColor(QPalette::Button, currentColor_);
    this->setPalette(pallete);
    this->update();
}

const QColor &ColorButton::color() const
{
    return currentColor_;
}

void ColorButton::callColorDialog()
{
    setColor(QColorDialog::getColor(currentColor_, nullptr, tr("Choose color")));
}
} // namespace dscontrol
