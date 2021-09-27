#include "ItemUISettings.h"

#include "ui_ItemUISettings.h"
#include "common/globals.h"

#include <QColorDialog>
#include <QFontDialog>

ItemUiSettings::ItemUiSettings(QWidget* parent)
: QGroupBox(parent)
, qss_ {}
, currentColor_ {}
, currentBackColor_ {}
, currentBorderColor_ {}
, currentFont_ {}
, ui_(new Ui::ItemUISettings)
{
    ui_->setupUi(this);
    qss_ = R"(
              QPushButton {
              border: none;
              background-color: curColor;
              }
             )";

    connect(ui_->colorButton, &QPushButton::clicked, this, &ItemUiSettings::colorButtonClicked);
    connect(ui_->backColorButton, &QPushButton::clicked, this, &ItemUiSettings::backColorButtonClicked);
    connect(ui_->borderColorButton, &QPushButton::clicked, this, &ItemUiSettings::borderColorButtonClicked);
    connect(ui_->fontButton, &QPushButton::clicked, this, &ItemUiSettings::fontButtonClicked);
}

ItemUiSettings::~ItemUiSettings()
{
    delete ui_;
}

const QColor ItemUiSettings::selectColor(const QColor &initColor)
{
    QColorDialog dialog(initColor);
    dialog.setStyleSheet(_DEF_STYLESHEET);
    return dialog.exec() == QDialog::Accepted ? dialog.currentColor() : initColor;
}

const QColor ItemUiSettings::setColor(QPushButton* button, const QColor &color)
{
    button->setIcon({});
    QString qss = qss_;
    button->setStyleSheet(qss.replace("curColor", color.name()));
    button->update();
    return color;
}

const QFont ItemUiSettings::selectFont(const QFont &initFont)
{
    QFontDialog dialog(initFont);
    dialog.setStyleSheet(_DEF_STYLESHEET);
    return dialog.exec() == QDialog::Accepted ? dialog.currentFont() : initFont;
}

const QFont ItemUiSettings::setFont(QLabel* label, const QFont &font)
{
    label->setFont(font);
    label->update();
    return font;
}

void ItemUiSettings::setValues(int borderWidth, const QColor &color, const QColor &backColor, const QColor &borderColor,
                               const QFont &font)
{
    ui_->widthSpinBox->setValue(borderWidth);
    currentColor_ = setColor(ui_->colorButton, color);
    currentBackColor_ = setColor(ui_->backColorButton, backColor);
    currentBorderColor_ = setColor(ui_->borderColorButton, borderColor);
    currentFont_ = setFont(ui_->fontLabel, font);
}

void ItemUiSettings::colorButtonClicked()
{
    currentColor_ = setColor(ui_->colorButton, selectColor(currentColor_));
}

void ItemUiSettings::backColorButtonClicked()
{
    currentBackColor_ = setColor(ui_->backColorButton, selectColor(currentBackColor_));
}

void ItemUiSettings::borderColorButtonClicked()
{
    currentBorderColor_ = setColor(ui_->borderColorButton, selectColor(currentBorderColor_));
}

void ItemUiSettings::fontButtonClicked()
{
    currentFont_ = setFont(ui_->fontLabel, selectFont(currentFont_));
}

int ItemUiSettings::borderWidth()
{
    return ui_->widthSpinBox->value();
}

QColor ItemUiSettings::color()
{
    return currentColor_;
}

QColor ItemUiSettings::backColor()
{
    return currentBackColor_;
}

QColor ItemUiSettings::borderColor()
{
    return currentBorderColor_;
}

QFont ItemUiSettings::font()
{
    return currentFont_;
}
