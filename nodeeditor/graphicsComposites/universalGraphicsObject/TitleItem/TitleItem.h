#pragma once

#include "common/nodeeditor_globals.hpp"

#include <QFont>
#include <QGraphicsItem>

namespace QtNodes
{
class LockSignItem;

class TitleItem : public QGraphicsItem
{
public:
    enum ViewMode
    {
        Normal,
        Underline
    };

public:
    TitleItem(Item::DeploymentType deploymentType, QGraphicsItem* parentItem = nullptr);
    QRectF boundingRect() const override;
    void setWidth(qreal width);
    qreal getWidth() const;
    void setFont(const QFont &font);
    void setColor(const QColor &color);
    void setText(const QString &text);
    void setViewMode(ViewMode viewMode);
    void setLocked(bool isLocked);

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    void computeHeight();
    void setHeight(qreal height);
    void correctText();
    void addDeploymentMark();

private:
    QRectF rect_;
    QColor color_;
    QFont font_;
    QString text_;
    QString drawingText_;
    ViewMode viewMode_;
    LockSignItem* lockSignItem_;
    Item::DeploymentType deploymentType_;
};
} // namespace QtNodes
