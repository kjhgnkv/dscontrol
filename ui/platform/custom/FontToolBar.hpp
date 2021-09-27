#pragma once

#include <QFontComboBox>
#include <QSpinBox>
#include <QToolBar>

#include "ColorButton.hpp"

namespace dscontrol
{
class ColorButton;

/*!
\brief qt designer have not functionality for add widgets in QToolBar, so
  we create custom class witch represent this
 */
class FontToolBar : public QToolBar
{
public:
    explicit FontToolBar(QWidget* parent);

    QFontComboBox* fontBox_;
    QSpinBox* fontSizeSpin_;
    ColorButton* fontColorButton_;
};
} // namespace dscontrol
