#pragma once

#include <QGroupBox>
#include <QPushButton>
#include <QLabel>

namespace Ui
{
class ItemUISettings;
}

class ItemUiSettings : public QGroupBox
{
Q_OBJECT

public:
    explicit ItemUiSettings(QWidget* parent = nullptr);
    ~ItemUiSettings();

    void setValues(int borderWidth, const QColor &color, const QColor &backColor, const QColor &borderColor,
                   const QFont &font);

    int borderWidth();
    QColor color();
    QColor backColor();
    QColor borderColor();
    QFont font();

private slots:
    void colorButtonClicked();
    void backColorButtonClicked();
    void borderColorButtonClicked();
    void fontButtonClicked();

private:
    const QColor selectColor(const QColor &initColor);
    const QColor setColor(QPushButton* button, const QColor &color);

    const QFont selectFont(const QFont &initFont);
    const QFont setFont(QLabel* label, const QFont &font);

private:
    QString qss_;
    QColor currentColor_;
    QColor currentBackColor_;
    QColor currentBorderColor_;
    QFont currentFont_;
    Ui::ItemUISettings* ui_;
};

