#pragma once

#include <QWidget>
#include <QtGui/QColor>

namespace QtNodes
{
/**\warning you can not inheritance this class, because in konstructor uses
 * virtual functions
 * \brief this class provide style for FlowView. You can change style only by
 * qss. Priority: backgroundSvg, backgroundPixmap, backgroundColor. If you do
 * not want draw grid - set backgroundColor, fineGridColor and coarseGridColor
 * in one value
 */
class FlowViewStyle final : public QWidget
{
Q_OBJECT
    Q_PROPERTY(QColor backgroundColor MEMBER backgroundColor NOTIFY notify)
    Q_PROPERTY(QColor fineGridColor MEMBER fineGridColor NOTIFY notify)
    Q_PROPERTY(QColor coarseGridColor MEMBER coarseGridColor NOTIFY notify)

    Q_PROPERTY(QString backgroundPixmap READ voidFun WRITE setBackgroundPixmap NOTIFY notify)

    Q_PROPERTY(QString backgroundSvg MEMBER svgImage NOTIFY notify)

public:
    FlowViewStyle();

    void setBackgroundPixmap(const QString &fileName);

    /**\return true if backgroundColor, fineGridColor, coarseGridColor not equal,
     * other way - false
     */
    bool drawGrid() const;

    QColor backgroundColor;
    QColor fineGridColor;
    QColor coarseGridColor;

    QString backgroundPixmapFileName;
    QPixmap backgroundPixmap;

    QString svgImage;

signals:
    void notify();

protected:
    /**\brief this method needed for Q_PROPERTY
     */
    QString voidFun() const
    {
        return {};
    };
};
} // namespace QtNodes
