#include "styles/NodeStyle.hpp"

#include <QDebug>

using QtNodes::NodeStyle;

NodeStyle::NodeStyle()
: normalBoundaryColor {255, 255, 255}
, hoveredBoundaryColor {255, 255, 255}
, pressedBoundaryColor {255, 165, 0}
, SelectedBoundaryColor {255, 165, 0}

, normalBackgroundColor {"gray"}
, hoveredBackgroundColor {"gray"}
, pressedBackgroundColor {"gray"}
, SelectedBackgroundColor {"gray"}

, shadowColor {20, 20, 20}

, headerFontColor {"white"}
, portFontColor {"white"}
, portFontColorFaded {"lightgray"}
, validationFontColor {"white"}

, connectionPointColor {169, 169, 169}
, filledConnectionPointColor {"cyan"}

, warningColor {128, 128, 0}
, errorColor {"red"}

, normalPenWidth {1.0}
, hoveredPenWidth {1.5}
, pressedPenWidth {1.5}
, SelectedPenWidth {1.5}

, connectionPointDiameter {8.0}
, opacity {0.8}
, titleBarBackgroundColor {"cyan"}
, portBarBackgroundColor {"lightgray"}
{
    headerFont.setStyleName("Bold");
    validationFont.setStyleName("Regular");
    portFont.setStyleName("Regular");
}

void NodeStyle::setHeaderFontFromString(const QString &str)
{
    headerFont.fromString(str);
    headerFont.setBold(true);
}

void NodeStyle::setPortFontFromString(const QString &str)
{
    portFont.fromString(str);
}

void NodeStyle::setValidationFontFromString(const QString &str)
{
    validationFont.fromString(str);
}

void NodeStyle::setNormalNodePixmap(const QString &str)
{
    normalNodePixmapFileName = str;
    normalNodePixmap.load(normalNodePixmapFileName);
}

void NodeStyle::setNormalNodeBorderMargins(const QString &str)
{
    normalNodeBorderMargins = marginsFromString(str);
}

void NodeStyle::setHoveredNodePixmap(const QString &str)
{
    hoveredNodePixmapFileName = str;
    hoveredNodePixmap.load(hoveredNodePixmapFileName);
}

void NodeStyle::setHoveredNodeBorderMargins(const QString &str)
{
    hoveredNodeBorderMargins = marginsFromString(str);
}

void NodeStyle::setPressedNodePixmap(const QString &str)
{
    pressedNodePixmapFileName = str;
    pressedNodePixmap.load(pressedNodePixmapFileName);
}

void NodeStyle::setPressedNodeBorderMargins(const QString &str)
{
    pressedNodeBorderMargins = marginsFromString(str);
}

void NodeStyle::setSelectedNodePixmap(const QString &str)
{
    SelectedNodePixmapFileName = str;
    SelectedNodePixmap.load(SelectedNodePixmapFileName);
}

void NodeStyle::setSelectedNodeBorderMargins(const QString &str)
{
    SelectedNodeBorderMargins = marginsFromString(str);
}

void NodeStyle::setTitleBarPixmap(const QString &str)
{
    titleBarPixmap.load(str);
}

void NodeStyle::setTitleBarMargins(const QString &str)
{
    titleBarMargins = marginsFromString(str);
}

void NodeStyle::setStatusBarPixmap(const QString &str)
{
    statusBarPixmap.load(str);
}

void NodeStyle::setStatusBarMargins(const QString &str)
{
    statusBarMargins = marginsFromString(str);
}

void NodeStyle::setPortBarPixmap(const QString &str)
{
    portBarPixmap.load(str);
}

void NodeStyle::setPortBarMargins(const QString &str)
{
    portBarMargins = marginsFromString(str);
}

namespace QtNodes
{
QString marginsToString(const QMargins &margins)
{
    QString retval;

    retval += QString::number(margins.left()) + ", ";
    retval += QString::number(margins.top()) + ", ";
    retval += QString::number(margins.right()) + ", ";
    retval += QString::number(margins.bottom());

    return retval;
}

QMargins marginsFromString(const QString &string)
{
    QMargins retval;

    int margins[4] {};
    auto marginsStrings = string.split(',');
    for (int i {}; i < 4 && i < marginsStrings.size(); ++i)
    {
        margins[i] = marginsStrings[i].toInt(nullptr, 0);
    }

    retval = QMargins {margins[0], margins[1], margins[2], margins[3]};
    return retval;
}
} // namespace QtNodes
