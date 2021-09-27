#include "ResizeBox.h"

#include "Corner.h"
#include "graphicsComposites/universalGraphicsObject/UniversalGraphicsItem.h"
#include "nodes/UniversalGraphicsItemImpl.h"

#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QSvgRenderer>
#include <models/NodesScene.hpp>
#include <singletones/Facade.hpp>

using namespace QtNodes;

ResizeBox::ResizeBox(QGraphicsItem* parent, int type)
: QObject {nullptr}
, QGraphicsItem {parent}
, rect_ {0, 0, 500, 500}
, path_ {}
, contentSize_ {}
, margin_ {8}
, currentMargin_ {margin_}
, borderWidth_ {margin_}
, curBorderWidth_ {borderWidth_}
, leftHasPressedFlag_ {false}
, color_ {"#2f80ed"}
, minSize_ {200, 200}
, type_ {type}
, curSizeScale_ {Small}
, canSideResize_ {true}
, canAngleResize_ {true}
, pictureResizeFlag_ {true}
, componentResizeFlag_ {true}
, textResizeFlag_ {true}
, savedRect_ {rect_}
, savedPos_ {-currentMargin_, -currentMargin_}
, hatchPixmap_ {":/ResizeBox.png"}
{
    setFlag(ItemSendsGeometryChanges, true);
    prepareCorners();

    path_.setFillRule(Qt::FillRule::OddEvenFill);
}

QRectF ResizeBox::boundingRect() const
{
    return rect_;
}

void ResizeBox::setColor(const QColor &color)
{
    color_ = color;
    update();
}

void ResizeBox::setType(Item::NodeType type)
{
    type_ = type;
}

void ResizeBox::setContentSize(const QSizeF &size)
{
    contentSize_ = size;
    updateRect();
    updateCorners();
}

QSizeF ResizeBox::getContentSize() const
{
    return contentSize_;
}

double ResizeBox::margin() const
{
    return currentMargin_;
}

void ResizeBox::setMinSize(const QSizeF &size)
{
    minSize_ = size + QSizeF {currentMargin_, currentMargin_} * 2;

    QPointF tmp;
    updateGeometry(Corner::Right, tmp);
}

void ResizeBox::setCurrentScale(qreal scale)
{
    for (int i = Corner::TopLeft; i <= Corner::Left; i++)
    {
        corners_[i]->setCurrentScale(scale);
    }

    if (scale > 100)
    {
        return;
    }

    qreal scaleRatio = 100 / scale;
    curBorderWidth_ = scaleRatio * borderWidth_;
    currentMargin_ = scaleRatio * margin_;

    updateRect();
    saveCurrentGeometry();
    updateGeometry();
}

void ResizeBox::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setPen(Qt::NoPen);
    painter->setBrush({color_, Qt::BDiagPattern});
    painter->drawPath(path_);
}

QVariant ResizeBox::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change)
    {
        case ItemVisibleChange:
        {
            if (value.toBool() == true)
            {
                updateCorners();
            }
            break;
        }
        default:
        {
            break;
        }
    }
    return QGraphicsItem::itemChange(change, value);
}

void ResizeBox::setLocked(bool isLocked)
{
    for (auto corner : corners_)
    {
        corner->setFlag(ItemIsMovable, !isLocked);
    }
}

void ResizeBox::prepareCorners()
{
    for (int cornerCode = Corner::TopLeft; cornerCode <= Corner::Left; cornerCode++)
    {
        Corner* corner = new Corner(static_cast<Corner::Code>(cornerCode), this);
        corners_.push_back(corner);

        connect(corner, &Corner::positionHasChanged, [=](Corner::Code cornerCode, const QPointF &deltaPos)
        {
            this->updateGeometry(cornerCode, deltaPos);
        });

        connect(corner, &Corner::movingHasStarted, [&]
        {
            saveCurrentGeometry();
            emit geometryChangeStarted();
        });

        connect(corner, &Corner::movingHasFinished, [&]
        {
            saveCurrentGeometry();
            emit geometryChangeFinished();
        });
    }

    updateCorners();
    updateCornerVisibility();
}

void ResizeBox::updateCorners()
{
    if (corners_.size() != 8)
    {
        return;
    }

    qreal width = rect_.width();
    qreal height = rect_.height();

    qreal dw = corners_[Corner::TopLeft]->width();

    qreal padding = currentMargin_ - dw;

    corners_[Corner::TopLeft]->setPos({padding, padding}, true);
    corners_[Corner::Top]->setPos({width / 2 - dw / 2, padding}, true);
    corners_[Corner::TopRight]->setPos({width - dw - padding, padding}, true);
    corners_[Corner::Right]->setPos({width - dw - padding, height / 2 - dw / 2}, true);
    corners_[Corner::BottomRight]->setPos({width - dw - padding, height - dw - padding}, true);
    corners_[Corner::Bottom]->setPos({width / 2 - dw / 2, height - dw - padding}, true);
    corners_[Corner::BottomLeft]->setPos({padding, height - dw - padding}, true);
    corners_[Corner::Left]->setPos({padding, height / 2 - dw / 2}, true);
}

void ResizeBox::updateGeometry(Corner::Code cornerCode, QPointF deltaPos)
{
    QSizeF newSize {computeNewSize(cornerCode, deltaPos)};

    correctDelta(cornerCode, deltaPos, newSize);

    const auto &newPos = computeNewPosition(cornerCode, deltaPos);
    setPos(newPos);

    setSize(newSize);

    updateCorners();
}

QSizeF ResizeBox::computeNewSize(int cornerCode, QPointF deltaPos)
{
    QSizeF curSize {savedRect_.size()};

    double dx {deltaPos.x()};
    double dy {deltaPos.y()};

    switch (cornerCode)
    {
        case Corner::TopLeft:
        {
            return {curSize.width() - dx, curSize.height() - dy};
        }
        case Corner::Top:
        {
            return {curSize.width(), curSize.height() - dy};
        }
        case Corner::TopRight:
        {
            return {curSize.width() + dx, curSize.height() - dy};
        }
        case Corner::Right:
        {
            return {curSize.width() + dx, curSize.height()};
        }
        case Corner::BottomRight:
        {
            return {curSize.width() + dx, curSize.height() + dy};
        }
        case Corner::Bottom:
        {
            return {curSize.width(), curSize.height() + dy};
        }
        case Corner::BottomLeft:
        {
            return {curSize.width() - dx, curSize.height() + dy};
        }
        case Corner::Left:
        {
            return {curSize.width() - dx, curSize.height()};
        }
    }
    return {};
}

void ResizeBox::correctDelta(int cornerCode, QPointF &deltaPos, QSizeF &newSize)
{
    QSizeF minSize {minSize_};

    QSizeF deltaSize {newSize - minSize};
    qreal deltaWidth {deltaSize.width()};
    qreal deltaHeight {deltaSize.height()};

    if (deltaWidth < 0)
    {
        switch (cornerCode)
        {
            case Corner::TopLeft:
            case Corner::BottomLeft:
            case Corner::Left:
            {
                deltaPos += {deltaWidth, 0};
                newSize += {-deltaWidth, 0};
                break;
            }
            case Corner::TopRight:
            case Corner::Right:
            case Corner::BottomRight:
            {
                deltaPos += {-deltaWidth, 0};
                newSize += {-deltaWidth, 0};
                break;
            }
        }
    }
    if (deltaHeight < 0)
    {
        switch (cornerCode)
        {
            case Corner::TopLeft:
            case Corner::Top:
            case Corner::TopRight:
            {
                deltaPos += {0, deltaHeight};
                newSize += {0, -deltaHeight};
                break;
            }
            case Corner::BottomRight:
            case Corner::Bottom:
            case Corner::BottomLeft:
            {
                deltaPos += {0, -deltaHeight};
                newSize += {0, -deltaHeight};
                break;
            }
        }
    }
}

QPointF ResizeBox::computeNewPosition(Corner::Code cornerCode, const QPointF &deltaPos)
{
    switch (cornerCode)
    {
        case Corner::TopLeft:
        {
            return savedPos_ + deltaPos;
        }
        case Corner::Top:
        case Corner::TopRight:
        {
            return savedPos_ + QPointF {0, deltaPos.y()};
        }
        case Corner::BottomLeft:
        case Corner::Left:
        {
            return savedPos_ + QPointF {deltaPos.x(), 0};
        }
        default:
            return savedPos_;
    }
}

void ResizeBox::enableResizing(bool canResize)
{
    if (canSideResize_ != canResize || canAngleResize_ != canResize)
    {
        canSideResize_ = canResize;
        canAngleResize_ = canResize;

        updateCornerVisibility();
    }
}

void ResizeBox::setViewMode(int mode)
{
    viewMode_ = mode;

    if (mode == UniversalGraphicsItemImpl::ViewMode::Picture)
        pictureResizeFlag_ = false;
    else
        pictureResizeFlag_ = true;

    updateCornerVisibility();
}

void ResizeBox::setComponentResize(bool flag)
{
    componentResizeFlag_ = flag;
    updateCornerVisibility();
}

void ResizeBox::setTextResize(bool flag)
{
    textResizeFlag_ = flag;
    updateCornerVisibility();
}

void ResizeBox::setCornerWidth(qreal width)
{
    for (auto corner : corners_)
        if (corner)
            corner->setWidth(width);
}

qreal ResizeBox::currentMargin() const
{
    return currentMargin_;
}

void ResizeBox::updateCornerVisibility()
{
    setVisibleSideCorners(canSideResize_ && pictureResizeFlag_ && componentResizeFlag_ && textResizeFlag_);
    setVisibleAngleCorners(canAngleResize_);
}

void ResizeBox::setVisibleSideCorners(bool flag)
{
    if (corners_.size() == 8)
    {
        if (auto corner = corners_[Corner::Top])
            corner->setVisible(flag);
        if (auto corner = corners_[Corner::Right])
            corner->setVisible(flag);
        if (auto corner = corners_[Corner::Bottom])
            corner->setVisible(flag);
        if (auto corner = corners_[Corner::Left])
            corner->setVisible(flag);
    }
}

void ResizeBox::setVisibleAngleCorners(bool flag)
{
    if (corners_.size() == 8)
    {
        if (auto corner = corners_[Corner::TopLeft])
            corner->setVisible(flag);
        if (auto corner = corners_[Corner::TopRight])
            corner->setVisible(flag);
        if (auto corner = corners_[Corner::BottomRight])
            corner->setVisible(flag);
        if (auto corner = corners_[Corner::BottomLeft])
            corner->setVisible(flag);
    }
}

void ResizeBox::updateRect()
{
    setSize({contentSize_.width() + currentMargin_ * 2, contentSize_.height() + currentMargin_ * 2});
    savedRect_ = rect_;
}

void ResizeBox::updatePath()
{
    auto right = rect_.right();
    auto bottom = rect_.bottom();

    path_ = {};

    path_.moveTo(0, 0);
    path_.lineTo(0, bottom);
    path_.lineTo(right, bottom);
    path_.lineTo(right, 0);
    path_.lineTo(0, 0);

    path_.moveTo(curBorderWidth_, curBorderWidth_);
    path_.lineTo(curBorderWidth_, bottom - curBorderWidth_);
    path_.lineTo(right - curBorderWidth_, bottom - curBorderWidth_);
    path_.lineTo(right - curBorderWidth_, curBorderWidth_);
    path_.lineTo(curBorderWidth_, curBorderWidth_);
}

void ResizeBox::setSize(const QSizeF &size)
{
    prepareGeometryChange();
    rect_.setSize(size);
    updatePath();
    contentSize_ = size - QSizeF(currentMargin_, currentMargin_) * 2;
}

void ResizeBox::saveCurrentGeometry()
{
    savedPos_ = {-currentMargin_, -currentMargin_};
    savedRect_ = rect_;
}
