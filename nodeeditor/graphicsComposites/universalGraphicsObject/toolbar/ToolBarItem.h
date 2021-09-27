#pragma once

#include <QGraphicsItem>

namespace QtNodes
{
class ToolBarWidget;

/*!
\brief Class represents graphics item for toolbar widget on the scene
*/
class ToolBarItem : public QObject, public QGraphicsItem
{
Q_OBJECT

public:
    ToolBarItem(QGraphicsItem* parent = nullptr);
    ~ToolBarItem() override;
    QRectF boundingRect() const override;
    /*!
     * \brief Return pointer on toolbar widget
     */
    ToolBarWidget* getToolBarWidget();

signals:
    void hasHidden();

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private:
    QRectF rect_;
    ToolBarWidget* toolBarWidget_;
    bool isFixedOnScene_;
};
} // namespace QtNodes
