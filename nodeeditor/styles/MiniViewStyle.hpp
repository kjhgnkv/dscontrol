// MiniViewStyle.hpp

#pragma once

#include <QWidget>

namespace QtNodes
{
class MiniViewStyle : public QWidget
{
Q_OBJECT
    Q_PROPERTY(QColor backgroundColor MEMBER backgroundColor)
    Q_PROPERTY(QColor shadowColor MEMBER shadowColor)

public:
    MiniViewStyle();

    QColor backgroundColor;
    QColor shadowColor;
};
} // namespace QtNodes
