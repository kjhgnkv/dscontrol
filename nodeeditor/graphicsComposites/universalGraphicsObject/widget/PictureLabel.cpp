#include "PictureLabel.h"

#include "nodes/UniversalGraphicsItemImpl.h"
#include <QPainter>
#include <QSvgRenderer>

namespace QtNodes
{
PictureLabel::PictureLabel(QWidget* parent)
: QLabel(parent)
, picture_ {}
, backColor_ {"#33304e"}
, borderRadius_ {0}
{
    updateStyleSheet();
}

void PictureLabel::setPicture(const QPixmap &pixmap)
{
    picture_ = pixmap;
}

void PictureLabel::setBackColor(const QColor &color)
{
    auto red = color.red();
    auto green = color.green();
    auto blue = color.blue();
    auto alpha = color.alpha();

    backColor_ = "rgba(" + QString::number(red) + ", " + QString::number(green) + ", " + QString::number(
    blue) + ", " + QString::number(alpha) + ")";
    updateStyleSheet();
}

void PictureLabel::paintEvent(QPaintEvent* event)
{
    QLabel::paintEvent(event);

    QPainter painter(this);

    if (!picture_.isNull())
    {
        int srcWidth = picture_.width();
        int srcHeight = picture_.height();
        int destWidth = width();
        int destHeight = height();

        double wCoef = static_cast<double>(destWidth) / srcWidth;
        double hCoef = static_cast<double>(destHeight) / srcHeight;
        double coef = std::min(wCoef, hCoef);

        double targetWidth = coef * srcWidth;
        double targetHeight = coef * srcHeight;

        double dx = (destWidth - targetWidth) / 2;
        double dy = (destHeight - targetHeight) / 2;

        QPixmap scaledPixmap(destWidth, destWidth);
        scaledPixmap.fill(Qt::transparent);

        QPainter scaledPainter(&scaledPixmap);
        scaledPainter.drawPixmap(QRectF {dx, dy, targetWidth, targetHeight}, picture_, picture_.rect());

        painter.setPen(Qt::NoPen);
        painter.setBrush(scaledPixmap);

        painter.drawRoundedRect(rect(), 11, 11);
    }
}

void PictureLabel::updateStyleSheet()
{
    QString qss = R"(

                  QLabel {
                      background-color: backColor;
                      border-radius: borderRadiuspx;
                  }

                  )";

    qss = qss.replace("backColor", backColor_);
    qss = qss.replace("borderRadius", QString::number(borderRadius_));

    setStyleSheet(qss);
}

} // namespace QtNodes
