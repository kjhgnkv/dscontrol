#include "LockSignItem.h"
#include "TitleItem.h"
#include "arrows/TinyMath.h"

#include <QPainter>
#include <QTextDocument>
#include <QtMath>

namespace QtNodes
{
TitleItem::TitleItem(Item::DeploymentType deploymentType, QGraphicsItem* parentItem)
: QGraphicsItem(parentItem)
, rect_ {0, 0, 100, 60}
, color_ {"#ffffff"}
, font_ {"Source Sans Pro", 14}
, text_ {}
, drawingText_ {}
, viewMode_ {ViewMode::Normal}
, lockSignItem_ {new LockSignItem {this}}
, deploymentType_ {deploymentType}
{
    computeHeight();
}

QRectF TitleItem::boundingRect() const
{
    return rect_;
}

void TitleItem::setWidth(qreal width)
{
    if (!TinyMath::almostEqual(width, rect_.width()))
    {
        prepareGeometryChange();
        rect_.setWidth(width);
        computeHeight();

        auto lockWidth = lockSignItem_->boundingRect().width();
        lockSignItem_->setPos(width / 2 - lockWidth / 2, lockSignItem_->y());
    }
}

qreal TitleItem::getWidth() const
{
    return rect_.width();
}

void TitleItem::setFont(const QFont &font)
{
    font_ = font;
    computeHeight();
}

void TitleItem::setColor(const QColor &color)
{
    color_ = color;
}

void TitleItem::setText(const QString &text)
{
    if (text == text_)
    {
        return;
    }

    text_ = text;
    correctText();
    computeHeight();
}

void TitleItem::setViewMode(TitleItem::ViewMode viewMode)
{
    if (viewMode == viewMode_)
        return;

    viewMode_ = viewMode;
    correctText();
}

void TitleItem::setLocked(bool isLocked)
{
    lockSignItem_->setLocked(isLocked);
}

void TitleItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setFont(font_);

    painter->setPen(color_);
    QTextOption textOption(Qt::AlignCenter);
    textOption.setWrapMode(QTextOption::WrapMode::WrapAtWordBoundaryOrAnywhere);
    painter->drawText(rect_, drawingText_, textOption);
}

void TitleItem::computeHeight()
{
    auto fm = QFontMetrics(font_);
    int width = fm.horizontalAdvance(drawingText_);
    int height = fm.height();

    auto rowCount = std::ceil(width / rect_.width());

    setHeight(height * rowCount);
}

void TitleItem::setHeight(qreal height)
{
    if (!TinyMath::almostEqual(height, rect_.height()))
    {
        prepareGeometryChange();
        rect_.setHeight(height);

        lockSignItem_->setPos(lockSignItem_->x(), height);
    }
}

void TitleItem::correctText()
{
    if (text_.isEmpty())
    {
        return;
    }

    if (viewMode_ == ViewMode::Normal)
    {
        drawingText_ = text_;
        addDeploymentMark();
    }
    else
    {
        drawingText_.clear();
        auto len = text_.length();
        for (auto i = 0; i < len; i++)
            drawingText_ += "_";
    }
}

void TitleItem::addDeploymentMark()
{
    drawingText_ += (deploymentType_ == Item::DeploymentType::Internal) ? " (I)" : " (E)";
}
} // namespace QtNodes
