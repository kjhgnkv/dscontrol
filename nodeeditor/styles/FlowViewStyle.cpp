#include "styles/FlowViewStyle.hpp"

using QtNodes::FlowViewStyle;

FlowViewStyle::FlowViewStyle()
: backgroundColor {34, 32, 51}
, fineGridColor {60, 60, 60}
, coarseGridColor {25, 25, 25}
{
}

void FlowViewStyle::setBackgroundPixmap(const QString &fileName)
{
    backgroundPixmapFileName = fileName;
    backgroundPixmap.load(backgroundPixmapFileName);
}

bool FlowViewStyle::drawGrid() const
{
    return (backgroundColor == fineGridColor && fineGridColor == coarseGridColor) ? false : true;
}
