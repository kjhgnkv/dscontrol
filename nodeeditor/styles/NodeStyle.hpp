#pragma once

#include <QWidget>
#include <QtGui/QColor>

namespace QtNodes
{
/**\warning you can not inheritance this class, because in konstructor uses
 * virtual functions
 */
class NodeStyle final : public QWidget
{
Q_OBJECT
    Q_PROPERTY(QColor normalBoundaryColor MEMBER normalBoundaryColor)
    Q_PROPERTY(QColor hoveredBoundaryColor MEMBER hoveredBoundaryColor)
    Q_PROPERTY(QColor pressedBoundaryColor MEMBER pressedBoundaryColor)
    Q_PROPERTY(QColor SelectedBoundaryColor MEMBER SelectedBoundaryColor)

    Q_PROPERTY(QColor normalBackgroundColor MEMBER normalBackgroundColor)
    Q_PROPERTY(QColor hoveredBackgroundColor MEMBER hoveredBackgroundColor)
    Q_PROPERTY(QColor pressedBackgroundColor MEMBER pressedBackgroundColor)
    Q_PROPERTY(QColor SelectedBackgroundColor MEMBER SelectedBackgroundColor)

    Q_PROPERTY(QColor shadowColor MEMBER shadowColor)

    Q_PROPERTY(QString headerFont READ funForSupportQProperty WRITE setHeaderFontFromString)
    Q_PROPERTY(QColor headerFontColor MEMBER headerFontColor)

    Q_PROPERTY(QString portFont READ funForSupportQProperty WRITE setPortFontFromString)
    Q_PROPERTY(QColor portFontColor MEMBER portFontColor)
    Q_PROPERTY(QColor portFontColorFaded MEMBER portFontColorFaded)

    Q_PROPERTY(QString validationFont READ funForSupportQProperty WRITE setValidationFontFromString)
    Q_PROPERTY(QColor validationFontColor MEMBER validationFontColor)

    Q_PROPERTY(QColor connectionPointColor MEMBER connectionPointColor)
    Q_PROPERTY(QColor filledConnectionPointColor MEMBER filledConnectionPointColor)
    Q_PROPERTY(QColor errorColor MEMBER errorColor)
    Q_PROPERTY(QColor warningColor MEMBER warningColor)

    Q_PROPERTY(QString normalNodePixmap READ funForSupportQProperty WRITE setNormalNodePixmap)
    Q_PROPERTY(QString normalNodeBorderMargins READ funForSupportQProperty WRITE setNormalNodeBorderMargins)

    Q_PROPERTY(QString hoveredNodePixmap READ funForSupportQProperty WRITE setHoveredNodePixmap)
    Q_PROPERTY(QString hoveredNodeBorderMargins READ funForSupportQProperty WRITE setHoveredNodeBorderMargins)

    Q_PROPERTY(QString pressedNodePixmap READ funForSupportQProperty WRITE setPressedNodePixmap)
    Q_PROPERTY(QString pressedNodeBorderMargins READ funForSupportQProperty WRITE setPressedNodeBorderMargins)

    Q_PROPERTY(QString SelectedNodePixmap READ funForSupportQProperty WRITE setSelectedNodePixmap)
    Q_PROPERTY(QString SelectedNodeBorderMargins READ funForSupportQProperty WRITE setSelectedNodeBorderMargins)

    Q_PROPERTY(float normalPenWidth MEMBER normalPenWidth)
    Q_PROPERTY(float hoveredPenWidth MEMBER hoveredPenWidth)
    Q_PROPERTY(float pressedPenWidth MEMBER pressedPenWidth)
    Q_PROPERTY(float SelectedPenWidth MEMBER SelectedPenWidth)

    Q_PROPERTY(float connectionPointDiameter MEMBER connectionPointDiameter)
    Q_PROPERTY(float opacity MEMBER opacity)

    Q_PROPERTY(QColor titleBarBackgroundColor MEMBER titleBarBackgroundColor)
    Q_PROPERTY(QString titleBarPixmap READ funForSupportQProperty WRITE setTitleBarPixmap)
    Q_PROPERTY(QString titleBarMargins READ funForSupportQProperty WRITE setTitleBarMargins)

    Q_PROPERTY(QString statusBarPixmap READ funForSupportQProperty WRITE setStatusBarPixmap)
    Q_PROPERTY(QString statusBarMargins READ funForSupportQProperty WRITE setStatusBarMargins)

    Q_PROPERTY(QColor portBarBackgroundColor MEMBER portBarBackgroundColor)
    Q_PROPERTY(QString portBarPixmap READ funForSupportQProperty WRITE setPortBarPixmap)
    Q_PROPERTY(QString portBarMargins READ funForSupportQProperty WRITE setPortBarMargins)

public:
    NodeStyle();

    void setHeaderFontFromString(const QString &str);
    void setPortFontFromString(const QString &str);
    void setValidationFontFromString(const QString &str);

    void setNormalNodePixmap(const QString &str);
    void setNormalNodeBorderMargins(const QString &str);

    void setHoveredNodePixmap(const QString &str);
    void setHoveredNodeBorderMargins(const QString &str);

    void setPressedNodePixmap(const QString &str);
    void setPressedNodeBorderMargins(const QString &str);

    void setSelectedNodePixmap(const QString &str);
    void setSelectedNodeBorderMargins(const QString &str);

    void setTitleBarPixmap(const QString &str);
    void setTitleBarMargins(const QString &str);

    void setStatusBarPixmap(const QString &str);
    void setStatusBarMargins(const QString &str);

    void setPortBarPixmap(const QString &str);
    void setPortBarMargins(const QString &str);

    /**\brief needed for support qproperty. Does nothing
     */
    QString funForSupportQProperty() const
    {
        return {};
    }

public:
    QColor normalBoundaryColor;
    QColor hoveredBoundaryColor;
    QColor pressedBoundaryColor;
    QColor SelectedBoundaryColor;

    QColor normalBackgroundColor;
    QColor hoveredBackgroundColor;
    QColor pressedBackgroundColor;
    QColor SelectedBackgroundColor;

    QColor shadowColor;

    QFont headerFont;
    QColor headerFontColor;

    QFont portFont;
    QColor portFontColor;
    QColor portFontColorFaded;

    QFont validationFont;
    QColor validationFontColor;

    QColor connectionPointColor;
    QColor filledConnectionPointColor;

    QColor warningColor;
    QColor errorColor;

    QString normalNodePixmapFileName;
    QPixmap normalNodePixmap;
    QMargins normalNodeBorderMargins;

    QString hoveredNodePixmapFileName;
    QPixmap hoveredNodePixmap;
    QMargins hoveredNodeBorderMargins;

    QString pressedNodePixmapFileName;
    QPixmap pressedNodePixmap;
    QMargins pressedNodeBorderMargins;

    QString SelectedNodePixmapFileName;
    QPixmap SelectedNodePixmap;
    QMargins SelectedNodeBorderMargins;

    float normalPenWidth;
    float hoveredPenWidth;
    float pressedPenWidth;
    float SelectedPenWidth;

    float connectionPointDiameter;

    float opacity;

    QColor titleBarBackgroundColor;
    QPixmap titleBarPixmap;
    QMargins titleBarMargins;

    QPixmap statusBarPixmap;
    QMargins statusBarMargins;

    QColor portBarBackgroundColor;
    QPixmap portBarPixmap;
    QMargins portBarMargins;
};

QString marginsToString(const QMargins &margins);
QMargins marginsFromString(const QString &string);

} // namespace QtNodes
