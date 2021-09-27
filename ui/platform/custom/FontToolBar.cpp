#include "FontToolBar.hpp"

#include "custom/ColorButton.hpp"
#include <QFontComboBox>
#include <QSpinBox>

#define MIN_SPIN_VAL 8
#define MAX_SPIN_VAL 72

namespace dscontrol
{
FontToolBar::FontToolBar(QWidget* parent)
: QToolBar {parent}
{
    fontBox_ = new QFontComboBox {this};
    fontSizeSpin_ = new QSpinBox {this};
    fontColorButton_ = new ColorButton {this};

    fontSizeSpin_->setMinimum(MIN_SPIN_VAL);
    fontSizeSpin_->setMaximum(MAX_SPIN_VAL);

    fontColorButton_->setStyleSheet("QPushButton {border: 1px solid #a9a9c3; border-radius: 6px; width: "
                                    "19px; height: 19px}");

    fontBox_->setStyleSheet("QComboBox {color: white; background-color: #3F3F47;  border: 1px "
                            "solid "
                            "#a9a9c3; border-radius: 5px;padding: 1px 18px 1px 3px;  min-width: "
                            "6em;}"
                            "QComboBox QAbstractItemView {background: #494953;"
                            "color: white;"
                            "selection-background-color: #292836;}"
                            "QComboBox::drop-down {subcontrol-origin: padding; "
                            "subcontrol-position: "
                            "top right; border-left-width: 1px; border-left-color: "
                            "#a9a9c3; border-left-style: solid; border-top-right-radius: 5px; "
                            "border-bottom-right-radius: 5px}"
                            "QComboBox::down-arrow {image: url(:/toolbar/mode/down);}");

    fontSizeSpin_->setStyleSheet("QSpinBox {color: white; background-color: #3F3F47;  border: 1px solid "
                                 "#a9a9c3; border-radius: 5px; selection-background-color: #292836}"
                                 "QSpinBox::up-button {background-color: #3F3F47; "
                                 "border-top-right-radius: 5px;}"
                                 "QSpinBox::down-button {background-color: #3F3F47; "
                                 "border-bottom-right-radius: 5px;}"
                                 "QSpinBox::up-arrow {image: url(:/toolbar/mode/up);}"
                                 "QSpinBox::down-arrow {image: url(:/toolbar/mode/down); }");

    this->addWidget(fontBox_);
    this->addWidget(fontSizeSpin_);
    this->addWidget(fontColorButton_);
}
} // namespace dscontrol
