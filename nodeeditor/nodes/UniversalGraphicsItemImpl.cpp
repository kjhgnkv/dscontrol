#include "UniversalGraphicsItemImpl.h"
#include "graphicsComposites/universalGraphicsObject/UniversalGraphicsItem.h"
#include "common/nodeeditor_globals.hpp"
#include "scenes/FlowScene.hpp"
#include "graphicsComposites/universalGraphicsObject/widget/UniversalGraphicsWidget.h"
#include <QJsonArray>
#include <QPainter>
#include <QSvgRenderer>

namespace QtNodes
{
UniversalGraphicsItemImpl::UniversalGraphicsItemImpl(const QtNodes::Item::DeploymentType &deploymentType,
                                                     const QString &caption)
: QtNodes::NodeImp(caption)
, widget_ {nullptr}
, innerScene_ {}
, parentScene_ {}
, viewMode_ {Scene}
, font_ {"Source Sans Pro", 14}
, fontDefault_ {font_}
, color_ {"#ffffff"}
, colorDefault_ {color_}
, backColor_ {"#33304e"}
, backColorDefault_ {backColor_}
, borderColor_ {"#605f6e"}
, borderColorDefault_ {borderColor_}
, borderWidth_ {1}
, borderWidthDefault_ {borderWidth_}
, picturePath_ {}
, minSize_ {180, 110}
, curMinSize_ {minSize_}
, size_ {minSize_}
, deploymentType_ {deploymentType}
, widgetMargin_ {26}
, type_ {int(QtNodes::Item::NodeType::Node)}
, portWidth_ {12}
, pos_ {0, 0}
, locked_ {false}
, isInnerLevelsHidden_ {false}
, hiddenTitleFlag_ {false}
, cornerWidth_ {8}
{
}

void UniversalGraphicsItemImpl::addWidget(UniversalGraphicsWidget* widget)
{
    if (!widget)
        return;

    if (widget_)
    {
        widget_->disconnect();
    }
    widget_ = widget;
    connect(widget_, &UniversalGraphicsWidget::titleResized, [this](const int width)
    {
        int maxWidth = std::max(int(minSize_.width()), width + widgetMargin_ * 2);
        curMinSize_.setWidth(maxWidth);

        emit minSizeChanged(curMinSize_);
    });
}

void UniversalGraphicsItemImpl::setPos(const QPointF &pos)
{
    pos_ = pos;
}

QPointF UniversalGraphicsItemImpl::getPos()
{
    return pos_;
}

void UniversalGraphicsItemImpl::setBackColor(const QColor &color)
{
    backColor_ = color;
}

const QColor UniversalGraphicsItemImpl::getBackColor()
{
    return backColor_;
}

void UniversalGraphicsItemImpl::setFontFamily(const QString &family)
{
    font_.setFamily(family);
}

const QString UniversalGraphicsItemImpl::getFontFamily()
{
    QString str = font_.family();
    return str;
}

void UniversalGraphicsItemImpl::setFontSize(const int &fontSize)
{
    font_.setPointSize(fontSize);
}

int UniversalGraphicsItemImpl::getFontSize()
{
    return font_.pointSize();
}

void UniversalGraphicsItemImpl::setFont(const QFont &font)
{
    font_ = font;
}

const QFont UniversalGraphicsItemImpl::getFont()
{
    return font_;
}

void UniversalGraphicsItemImpl::setColor(const QColor &color)
{
    color_ = color;
}

const QColor UniversalGraphicsItemImpl::getColor()
{
    return color_;
}

void UniversalGraphicsItemImpl::setItalic(bool flag)
{
    font_.setItalic(flag);
}

bool UniversalGraphicsItemImpl::isItalic()
{
    return font_.italic();
}

void UniversalGraphicsItemImpl::setInnerScene(const QUuid &scene)
{
    innerScene_ = scene;
}

const QUuid UniversalGraphicsItemImpl::getInnerScene() const
{
    return innerScene_;
}

void UniversalGraphicsItemImpl::setParentScene(const QUuid &scene)
{
    parentScene_ = scene;
}

QUuid UniversalGraphicsItemImpl::getParentScene() const
{
    return parentScene_;
}

void UniversalGraphicsItemImpl::setUnderline(bool flag)
{
    font_.setUnderline(flag);
}

bool UniversalGraphicsItemImpl::isUnderline()
{
    return font_.underline();
}

void UniversalGraphicsItemImpl::setBorderColor(const QColor &color)
{
    borderColor_ = color;
}

const QColor UniversalGraphicsItemImpl::getBorderColor()
{
    return borderColor_;
}

void UniversalGraphicsItemImpl::setBorderWidth(const int &width)
{
    borderWidth_ = width;
}

int UniversalGraphicsItemImpl::getBorderWidth()
{
    return borderWidth_;
}

void UniversalGraphicsItemImpl::setFontWeight(const int &weight)
{
    font_.setWeight(weight);
}

int UniversalGraphicsItemImpl::getFontWeight()
{
    return font_.weight();
}

void UniversalGraphicsItemImpl::setPicture(const QString &path)
{
    picturePath_ = path;
}

const QPixmap UniversalGraphicsItemImpl::picture() const
{
    return {picturePath_};
}

const QString UniversalGraphicsItemImpl::picturePath() const
{
    return picturePath_;
}

const QPixmap UniversalGraphicsItemImpl::hiddenPicture() const
{
    QSvgRenderer renderer(QString {":/pictures/hidden.svg"});

    int min = int(std::min(size_.width(), size_.height()));

    QPixmap pm(min, min);
    pm.fill(Qt::transparent);

    QPainter painter(&pm);
    renderer.render(&painter, pm.rect().translated(0, min / 18));

    return pm;
}

void UniversalGraphicsItemImpl::setViewMode(const int &mode)
{
    viewMode_ = mode;
}

int UniversalGraphicsItemImpl::viewMode()
{
    return viewMode_;
}

void UniversalGraphicsItemImpl::setMinSize(const QSizeF &size)
{
    minSize_ = size;
}

const QSizeF UniversalGraphicsItemImpl::getMinSize()
{
    return curMinSize_;
}

void UniversalGraphicsItemImpl::setLocked(bool flag)
{
    locked_ = flag;
}

bool UniversalGraphicsItemImpl::isLocked() const
{
    return locked_;
}

void UniversalGraphicsItemImpl::setSize(const QSizeF size)
{
    size_ = size;
}

QSizeF UniversalGraphicsItemImpl::getSize()
{
    return size_;
}

QRectF UniversalGraphicsItemImpl::getWidgetGeometry()
{
    QRectF rect(widgetMargin_, widgetMargin_, size_.width() - widgetMargin_ * 2, size_.height() - widgetMargin_ * 2);
    return rect;
}

void UniversalGraphicsItemImpl::setDeploymentType(const QtNodes::Item::DeploymentType &deploymentType)
{
    deploymentType_ = deploymentType;
    emit deploymentTypeChanged();
}

QtNodes::Item::DeploymentType UniversalGraphicsItemImpl::deploymentType() const
{
    return deploymentType_;
}

bool UniversalGraphicsItemImpl::isTitleVisible() const
{
    return hiddenTitleFlag_;
}

void UniversalGraphicsItemImpl::setTitleVisible(bool flag)
{
    hiddenTitleFlag_ = flag;
}

QColor UniversalGraphicsItemImpl::borderColorDefault()
{
    return borderColorDefault_;
}

qreal UniversalGraphicsItemImpl::getCornerWidth() const
{
    return cornerWidth_;
}

int UniversalGraphicsItemImpl::getWidgetMinWidth()
{
    if (!widget_)
        return 0;
    return widget_->minimumWidth() + widgetMargin_ * 2;
}

int UniversalGraphicsItemImpl::getWidgetMinHeight()
{
    if (!widget_)
        return 0;
    return widget_->minimumHeight() + widgetMargin_ * 2;
}

Item::NodeType UniversalGraphicsItemImpl::getType() const
{
    return type_;
}

void UniversalGraphicsItemImpl::resetFormat()
{
    font_ = fontDefault_;
    color_ = colorDefault_;
    backColor_ = backColorDefault_;
    borderColor_ = borderColorDefault_;
    borderWidth_ = borderWidthDefault_;
}

double UniversalGraphicsItemImpl::portWidth() const
{
    return portWidth_;
}

QJsonObject UniversalGraphicsItemImpl::save() const
{
    QJsonObject retval = NodeImp::save();

    retval["caption"] = caption();

    retval["innerScene"] = innerScene_.toString();
    retval["parentScene"] = parentScene_.toString();

    QJsonObject obj;

    obj["x"] = pos_.x();
    obj["y"] = pos_.y();

    retval["pos"] = obj;

    obj = {};

    obj["width"] = minSize_.width();
    obj["height"] = minSize_.height();

    retval["minSize"] = obj;

    obj = {};

    obj["width"] = size_.width();
    obj["height"] = size_.height();
    retval["size"] = obj;

    retval["fontFamily"] = font_.family();
    retval["fontSize"] = font_.pointSize();
    retval["isItalic"] = font_.italic();
    retval["isUnderline"] = font_.underline();
    retval["fontWeight"] = font_.weight();
    retval["color"] = color_.name();
    retval["backColor"] = backColor_.name();
    retval["borderColor"] = borderColor_.name();
    retval["borderWidth"] = borderWidth_;
    retval["pixmap"] = picturePath_;
    retval["deploymentType"] = (int)deploymentType_;
    retval["widgetMargin"] = widgetMargin_;
    retval["type"] = int(type_);
    retval["viewMode"] = viewMode_;
    retval["locked"] = int(locked_);
    retval["isInnerLevelsHidde"] = isInnerLevelsHidden_;

    return retval;
}

void UniversalGraphicsItemImpl::restore(const QJsonObject &p)
{
    NodeImp::restore(p);
    setParentScene(QUuid::fromString(p["parentScene"].toString()));
    QJsonObject tmp = p["minSize"].toObject();
    setMinSize({tmp["width"].toDouble(), tmp["height"].toDouble()});
    tmp = p["pos"].toObject();
    setPos({tmp["x"].toDouble(), tmp["y"].toDouble()});
    tmp = p["size"].toObject();
    setSize({tmp["width"].toDouble(), tmp["height"].toDouble()});
    setCaption(p["caption"].toString());
    setFontFamily(p["fontFamily"].toString());
    setFontSize(p["fontSize"].toInt());
    setItalic(p["isItalic"].toBool());
    setUnderline(p["isUnderline"].toBool());
    setFontWeight(p["fontWeight"].toInt());
    setColor(p["color"].toString());
    setBackColor(p["backColor"].toString());
    setBorderColor(p["borderColor"].toString());
    setBorderWidth(p["borderWidth"].toInt());
    setPicture(p["pixmap"].toString());
    setDeploymentType((QtNodes::Item::DeploymentType)p["deploymentType"].toInt());
    widgetMargin_ = p["widgetMargin"].toInt();
    type_ = static_cast<Item::NodeType>(p["type"].toInt());
    setViewMode(p["viewMode"].toInt());
    setLocked(bool(p["locked"].toInt()));
    isInnerLevelsHidden_ = p["isInnerLevelsHidde"].toBool();
}
} // namespace QtNodes
