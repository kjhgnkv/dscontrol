#include "Icon.hpp"

#include <QPainter>
#include <QDebug>

using namespace monitor;

Icon::Icon()
{
    px_ = new QPixmap(20, 20);
    setIconColor(IconColor::Green);
}

Icon::~Icon()
{
    delete px_;
}

void Icon::iconRed()
{
    px_->fill(Qt::transparent);

    QPainter painter = QPainter(px_);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect pxsize = px_->rect().adjusted(5, 5, -5, -5);

    painter.setBrush(QColor(Qt::red));
    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawEllipse(pxsize);

    painter.end();
}

void Icon::iconGreen()
{
    px_->fill(Qt::transparent);

    QPainter painter = QPainter(px_);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect pxsize = px_->rect().adjusted(5, 5, -5, -5);

    painter.setBrush(QColor(Qt::green));
    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawEllipse(pxsize);

    painter.end();
}

void Icon::setIconColor(IconColor value)
{
    switch (value)
    {
        case IconColor::Red:
        {
            iconRed();
            break;
        }
        case IconColor::Green:
        {
            iconGreen();
            break;
        }
    }
}

QPixmap* Icon::px() const
{
    return px_;
}
