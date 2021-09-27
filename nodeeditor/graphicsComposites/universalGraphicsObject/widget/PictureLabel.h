#pragma once

#include <QLabel>

namespace QtNodes
{
class PictureLabel : public QLabel
{
public:
    PictureLabel(QWidget* parent = nullptr);
    void setPicture(const QPixmap &pixmap);
    void setBackColor(const QColor &color);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void updateStyleSheet();

private:
    QPixmap picture_;
    QString backColor_;
    int borderRadius_;
};
} // namespace QtNodes
