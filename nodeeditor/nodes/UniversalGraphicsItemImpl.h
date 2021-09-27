#pragma once

#include "QUuid"
#include "nodeeditor/nodeeditor_export.h"
#include "common/nodeeditor_globals.hpp"
#include "nodes/NodeImp.hpp"
#include <QFont>
#include <QPixmap>
#include <QString>

namespace QtNodes
{
class FlowScene;
class UniversalGraphicsWidget;

/*!
\brief Class represents UniversalGraphicsItem implementation (item properties
storage)

Inherits by: SystemItemImpl, SubSystemItemImpl, ComputerItemImpl,
ApplicationItemImpl, ComponentItemImpl
*/
class NODEEDITOR_EXPORT UniversalGraphicsItemImpl : public QtNodes::NodeImp
{
Q_OBJECT

public:
    /*!
    \brief UniversalGraphicsWidget display types
    Scene - displays inner scene
    Picture - displays picture
    Component - ?
    */
    enum ViewMode
    {
        Scene,
        Picture,
        Hidden,
        Component
    };

public:
    /*!
    \param deploymentType item deployment type
    */
    UniversalGraphicsItemImpl(const QtNodes::Item::DeploymentType &deploymentType, const QString &caption = tr("Node"));
    /*!
    \brief Sets item position
    */
    void setPos(const QPointF &pos);
    /*!
    \return item position
    */
    QPointF getPos();
    /*!
    \brief sets up item widget
    */
    void addWidget(UniversalGraphicsWidget* widget);
    /*!
    \brief Sets item background color
    */
    void setBackColor(const QColor &color);
    /*!
    \return item background color
    */
    const QColor getBackColor();
    /*!
    \brief Sets item font family
    */
    void setFontFamily(const QString &family);
    /*!
    \return item font family
    */
    const QString getFontFamily();
    /*!
    \brief Sets item font size
    */
    void setFontSize(const int &fontSize);
    /*!
    \return item font size
    */
    int getFontSize();
    /*!
    \brief Sets item font weight
    */
    void setFontWeight(const int &weight);
    /*!
    \return item font weight
    */
    int getFontWeight();
    /*!
    \brief Sets item font
    */
    void setFont(const QFont &font);
    /*!
    \return item font
    */
    const QFont getFont();
    /*!
    \brief Sets item font color
    */
    void setColor(const QColor &color);
    /*!
    \return item font color
    */
    const QColor getColor();
    /*!
    \brief Sets item font italic property
    */
    void setItalic(bool flag = true);
    /*!
    \return item font italic property
    */
    bool isItalic();
    void setInnerScene(const QUuid &scene);
    const QUuid getInnerScene() const;
    void setParentScene(const QUuid &scene);
    QUuid getParentScene() const;
    /*!
    \brief Sets item font underline property
    */
    void setUnderline(bool flag = true);
    /*!
    \return item font underline property
    */
    bool isUnderline();
    /*!
    \brief Sets item border color
    */
    void setBorderColor(const QColor &color);
    /*!
    \return item border color
    */
    const QColor getBorderColor();
    /*!
    \brief Sets item border width
    */
    void setBorderWidth(const int &width);
    /*!
    \return item border width
    */
    int getBorderWidth();
    /*!
    \brief Sets item picture
    */
    void setPicture(const QString &path);
    /*!
    \return item picture
    */
    const QPixmap picture() const;
    /*!
    \return item picture path
    */
    const QString picturePath() const;

    /*!
    \return item picture
    */
    const QPixmap hiddenPicture() const;
    /*!
    \brief Sets item view mode
    */
    void setViewMode(const int &viewMode);
    /*!
    \return item view mode
    */
    int viewMode();
    /*!
    \brief Sets item minimal size
    */
    void setMinSize(const QSizeF &size);
    /*!
    \return item minimal size
    */
    const QSizeF getMinSize();
    /*!
    \brief Sets item size
    */
    void setSize(const QSizeF size);
    /*!
    \return item size
    */
    QSizeF getSize();
    /*!
    \return item widget geometry
    */
    QRectF getWidgetGeometry();
    /*!
    \brief Sets item deployment type
    */
    void setDeploymentType(const QtNodes::Item::DeploymentType &deploymentType) override;
    /*!
    \return item widget minimal size
    */
    int getWidgetMinWidth();
    /*!
    \return item widget minimal height
    */
    int getWidgetMinHeight();
    /*!
    \return item type
    */
    Item::NodeType getType() const;
    /*!
    \brief reset item properties to default values
    */
    void resetFormat();
    /*!
    \return item port width
    */
    double portWidth() const;
    /*!
    \brief Sets item lock status
    */
    void setLocked(bool flag = true);
    /*!
     \return item lock status
     */
    bool isLocked() const;

    QtNodes::Item::DeploymentType deploymentType() const override;

    /*!
     * \brief Get visibility status of title
     * \return True or false-value
     */
    bool isTitleVisible() const;
    /*!
     * \brief Set visibility status of title
     */
    void setTitleVisible(bool flag);

    QColor borderColorDefault();
    /*!
     * \brief Getting of corner width for different node types
     * \return Corner width
     */
    qreal getCornerWidth() const;

signals:
    void minSizeChanged(const QSizeF);
    void deploymentTypeChanged();
    // void itemPropertiesChanged();

protected:
    UniversalGraphicsWidget* widget_;
    QUuid innerScene_;
    QUuid parentScene_;
    int viewMode_;
    QFont font_, fontDefault_;
    QColor color_, colorDefault_;
    QColor backColor_, backColorDefault_;
    QColor borderColor_, borderColorDefault_;
    int borderWidth_, borderWidthDefault_;
    QString picturePath_;
    QSizeF minSize_, curMinSize_, size_;
    QtNodes::Item::DeploymentType deploymentType_;
    int widgetMargin_;
    Item::NodeType type_;
    double portWidth_;
    QPointF pos_;
    bool locked_;
    bool isInnerLevelsHidden_;
    bool hiddenTitleFlag_;
    qreal cornerWidth_;

public:
    QJsonObject save() const;
    void restore(const QJsonObject &p);
};
} // namespace QtNodes
