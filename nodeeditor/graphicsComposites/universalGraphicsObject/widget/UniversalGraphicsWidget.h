#pragma once

#include "views/FlowView.hpp"
#include <QWidget>

namespace Ui
{
class UniversalGraphicsWidget;
}

class QSvgWidget;
class QLabel;

namespace QtNodes
{
class FlowScene;
class FlowView;

/*!
\brief Class represents central widget of UniversalGraphicsItem
*/
class UniversalGraphicsWidget : public QWidget
{
Q_OBJECT

public:
    /*!
    \param scene scene of widget view
    \param scaleCoef scaling coefficient of widget view (now not used)
    */
    UniversalGraphicsWidget(QtNodes::FlowScene* scene, QWidget* parent = nullptr);
    ~UniversalGraphicsWidget();

    FlowView* getFlowView();

    /*!
    \brief Sets widget background color
    */
    void setBackColor(const QColor &color);
    /*!
    \brief Sets widget picture
    */
    void setPicture(const QPixmap &pixmap);
    /*!
    \brief Sets widget view mode (UniversalGraphicsItemImpl::ViewMode)
    */
    void setViewMode(int mode);
    /*!
    \return scene of widget view
    */
    QtNodes::FlowScene* nestedScene();
    /*!
    \brief Sets widgets view scene
    */
    void setScene(QtNodes::FlowScene* scene);
    /*!
    \brief Sets widget geometry
    */
    void setGeometry(QRect rect);
    /*!
    \return widgets view
    */
    QtNodes::FlowView* nestedView();
    /*!
     * \brief Set picture background color
     * \param Color
     */
    void setPictureBackColor(const QColor &color);

signals:
    /*!
    \brief emits when widget title resizes
    */
    void titleResized(const int);

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    void moveTitle();

private:
    QtNodes::FlowScene* scene_;
    Ui::UniversalGraphicsWidget* ui_;
};
} // namespace QtNodes
