#pragma once

#include "ToolBarWidget.h"

namespace QtNodes
{
struct ToolBarWidget::Properties
{
public:
    QFont font;
    QColor color;
    QColor backColor;
    QColor borderColor;
    qint32 borderWidth;
};
}
