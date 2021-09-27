#pragma once

#include <QPushButton>

namespace dscontrol
{
/*!
\brief custom button with possibility to set background color
 */
class ColorButton final : public QPushButton
{
Q_OBJECT
public:
    explicit ColorButton(QWidget* parent);

    /*!
    \brief set current color and change background of button
     */
    void setColor(const QColor &color);
    const QColor &color() const;

private slots:
    void callColorDialog();

private:
    QColor currentColor_;
};
} // namespace dscontrol
