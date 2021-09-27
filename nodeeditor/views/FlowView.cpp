#include "FlowView.hpp"

#include "graphicsComposites/universalGraphicsObject/UniversalGraphicsItem.h"
#include "arrows/AbstractArrow.h"
#include "graphicsComposites/frame/FrameGraphicsItem.hpp"
#include "models/NodesScene.hpp"
#include "scenes/FlowScene.hpp"
#include "singletones/Facade.hpp"
#include "styles/StyleCollection.hpp"
#include "views/FlowViewPainter.hpp"
#include "common/ArrowController.h"
#include "graphicsComposites/universalGraphicsObject/ports/UniversalPortProjection.h"

#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QMenu>
#include <QRubberBand>
#include <QTimer>
#include <QToolTip>
#include <QWheelEvent>
#include <cmath>

#define MAX_SCALE 500
#define MIN_SCALE 20
#define STEP 10
#define MINI_SCALE 30

namespace QtNodes
{
FlowView::FlowView(QWidget* parent)
: QGraphicsView(parent)
, cutterMode_ {false}
, scene_ {nullptr}
, deleteSelectionAction_ {nullptr}
, normalizeSelectionAction_ {nullptr}
, backColor_ {"#33304e"}
, rubberBand_ {nullptr}
{
    qDebug() << "FlowView: constructor";

    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setAttribute(Qt::WA_AlwaysShowToolTips);

    // TODO I off cache because with it draw uncorrectly
    setCacheMode(QGraphicsView::CacheBackground);

    // if style changed by qproperty in general cikle emit signal about this
    // and we have to update out view
    // TODO I don't know why, but here I can not connect signal with slot - it
    // is really strange
    connect(&(StyleCollection::instance().flowViewStyle()), &QtNodes::FlowViewStyle::notify, this, [this]()
    {
        this->update();
    });
    currentScale_ = 100;

    connect(Facade::Instance().sceneModel(), &NodesScene::topViewChanged, [this](const FlowView* oldValue)
    {
        if (this == Facade::Instance().sceneModel()->getTopView())
        {
            setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
            setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        }
        else if (this == oldValue)
        {
            setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    });

    setStyleSheet(R"(
                  QGraphicsView{ background-color: #33304e; margin: 8px;}
                    QToolTip {
                        background-color: white;
                        color: black;
                        border: 1px solid black;
                        font-family: Source Sans Pro;
                        font-size: 12px;
                    }
                  )");

    // TODO SCALE REFACTORING old code. test and remove
    /*connect(this, &FlowView::scaleChanged, [=] {
        checkScaleForItemTitles();
        changeScaleForItemToolBar();
        changeScaleForResizableItems(currentScale);

        // emit itemsUnselected(currentScale);
    });*/

    // TODO SCALE REFACTORING new code
    connect(this, &FlowView::scaleChanged, this, &FlowView::changeScaleForResizableItems);

    connect(this, &FlowView::scaleChanged, this, &FlowView::changeTextItemsVisibility);

    setAttribute(Qt::WA_AlwaysShowToolTips);
}

FlowView::~FlowView()
{
    if (rubberBand_)
        delete rubberBand_;
}

void FlowView::setCutterMode(bool b)
{
    cutterMode_ = b;
}

QAction* FlowView::deleteSelectionAction() const
{
    qDebug() << "FlowView: delete Selected action";
    return deleteSelectionAction_;
}

QAction* FlowView::normalizeSelectionAction() const
{
    qDebug() << "FlowView: normalize selection action";
    return normalizeSelectionAction_;
}

void FlowView::setFlowScene(FlowScene* scene)
{
    qDebug() << "FlowView: set flow scene";
    if (scene_)
    {
        disconnect(scene_, 0, this, 0);
        disconnect(this, 0, scene_, 0);
    }
    scene_ = scene;
    if (scene_)
    {
        setAlignment(Qt::AlignTop | Qt::AlignLeft);

        QGraphicsView::setScene(scene_);
        scene_->updateSceneRect();

        if (scene_->type() == Item::NodeType::MainScene)
        {
            qDebug() << scene_->barPos();
            qDebug() << horizontalScrollBar()->value();
            // TODO
            horizontalScrollBar()->setValue(qRound(scene_->barPos().x()));
            verticalScrollBar()->setValue(qRound(scene_->barPos().y()));
            qDebug() << horizontalScrollBar()->value();
            qDebug() << verticalScrollBar()->value();
        }

        connect(scene_, &QGraphicsScene::destroyed, this, [this]()
        {
            scene_ = nullptr;
        });

        delete deleteSelectionAction_;
        //         deleteSelectionAction_ =
        //             new QAction(QStringLiteral("Delete Selection"), this);
        //         deleteSelectionAction_->setShortcut(Qt::Key_Delete);
        //         connect(deleteSelectionAction_,
        //                 &QAction::triggered,
        //                 scene_,
        //                 &FlowScene::deleteSelectedNodes);
        //         addAction(deleteSelectionAction_);

        delete normalizeSelectionAction_;
        normalizeSelectionAction_ = new QAction {QAction::tr("Normilize Selection"), this};
    }
}

void FlowView::contextMenuEvent(QContextMenuEvent* event)
{
    qDebug() << "FlowView: context menu event";
    QGraphicsView::contextMenuEvent(event);
}

void FlowView::wheelEvent(QWheelEvent* event)
{
    if (QApplication::keyboardModifiers() & Qt::ControlModifier)
    {
        // TODO: uncomment if you want to zoom views inside graphics items
        //         if (scene()->items(mapToScene(event->pos())).empty())
        //         {
        QPoint delta = event->angleDelta();

        if (delta.y() == 0)
        {
            event->ignore();
            return;
        }

        double const d = delta.y() / std::abs(delta.y());

        if (d > 0.0)
        {
            scaleUp();
        }
        else
        {
            scaleDown();
        }
    }
    else
    {
        if (this == Facade::Instance().sceneModel()->getTopView())
        {
            QGraphicsView::wheelEvent(event);
        }
    }
}

void FlowView::scaleUp()
{
    qDebug() << "FlowView: scale up";
    if (currentScale_ < MAX_SCALE)
    {
        int step = STEP;
        if (currentScale_ > 100)
        {
            step *= 2;
        }

        // currentScale *= step;
        if ((currentScale_ + step) > MAX_SCALE && (currentScale_ + STEP) > MAX_SCALE)
        {
            return;
        }
        if ((currentScale_ + step) > MAX_SCALE)
        {
            step = STEP;
        }

        double factor = (double(currentScale_) + step) / currentScale_;
        currentScale_ += step;

        scale(factor, factor);

        emit visualizedRectChanged();
        emit scaleChanged(currentScale_);
    }
}

void FlowView::scaleDown()
{
    qDebug() << "FlowView: scale down";
    if (currentScale_ > MIN_SCALE)
    {
        if (scene_)
        {
            int step = STEP;
            if (currentScale_ > 100)
            {
                step *= 2;
            }
            double factor {};

            if ((currentScale_ - step) < MIN_SCALE && (currentScale_ - STEP) < MIN_SCALE)
            {
                return;
            }

            if ((currentScale_ - step) < MIN_SCALE)
            {
                step = STEP;
            }

            factor = (double(currentScale_) - step) / currentScale_;
            currentScale_ -= step;

            scale(factor, factor);

            scene_->updateSceneRect();

            emit visualizedRectChanged();
            emit scaleChanged(currentScale_);
        }
    }
}

void FlowView::setScale(int scale)
{
    if (scale >= MIN_SCALE && scale <= MAX_SCALE && scale != currentScale_)
    {
        if (scene_)
        {
            double factor = static_cast<double>(scale) / currentScale_;

            this->scale(factor, factor);
            currentScale_ = scale;

            if (factor < 1)
            {
                scene_->updateSceneRect();
            }

            emit visualizedRectChanged();
            emit scaleChanged(currentScale_);
        }
    }
}

void FlowView::mousePressEvent(QMouseEvent* event)
{
    if (nullptr == scene_)
        return;

    if (cutterMode_)
    {
        cutterOrigin_ = event->pos();
        if (!rubberBand_)
        {
            rubberBand_ = new QRubberBand(QRubberBand::Rectangle, this);
            QPalette pal;
            pal.setBrush(QPalette::Highlight, QBrush(Qt::gray));
            rubberBand_->setPalette(pal);
        }
        rubberBand_->setGeometry(QRect(cutterOrigin_, QSize()));
        rubberBand_->show();
        return;
    }
    if (auto item = scene_->itemAt(mapToScene(event->pos()), QTransform()))
    {
        grabItemOldRect_ = item->sceneBoundingRect();
    }
    else if (Qt::LeftButton == event->button())
    {
        if (this == Facade::Instance().sceneModel()->getTopView())
        {
            setDragMode(QGraphicsView::RubberBandDrag);
            scene_->setDragMode(FlowScene::DragMode::RubberBand);
        }
        else
        {
            setDragMode(QGraphicsView::NoDrag);
            scene_->setDragMode(FlowScene::DragMode::NoDrag);

            emit itemsUnselected(currentScale_);
        }
    }
    else if (event->button() == Qt::MiddleButton)
    {
        setDragMode(QGraphicsView::ScrollHandDrag);

        event = new QMouseEvent(QEvent::GraphicsSceneMousePress, event->pos(), Qt::MouseButton::LeftButton,
                                Qt::MouseButton::LeftButton, Qt::KeyboardModifier::NoModifier);
    }

    QGraphicsView::mousePressEvent(event);
}

void FlowView::mouseMoveEvent(QMouseEvent* event)
{
    if (nullptr != scene_ && nullptr != event)
    {
        if (cutterMode_ && rubberBand_ && event->buttons() == Qt::LeftButton)
        {
            rubberBand_->setGeometry(QRect(cutterOrigin_, event->pos()).normalized());
            return;
        }
        if (nullptr == scene_->mouseGrabberItem() && event->buttons() == Qt::LeftButton && (event->modifiers() & Qt::ShiftModifier) == 0)
        {
            emit visualizedRectChanged();
        }
        scene_->update();
    }

    QGraphicsView::mouseMoveEvent(event);
}

void FlowView::showEvent(QShowEvent* event)
{
    /**
        This line has been comment for the VIDE-255 task by Nick.
        "After reset image disappears part of connection."
    **/
    /**
        This line has been uncomment for the VIDE-293 task by Yura.
        "�� ������������ ������� connection �� separate scene"
    **/
    // TODO Test it
    showTopLevelConnections(false);
    // emit scaleChanged(currentScale_);
    QGraphicsView::showEvent(event);
}

FlowScene* FlowView::flowScene() const
{
    return scene_;
}

void FlowView::resizeEvent(QResizeEvent* event)
{
    qDebug() << "FlowView: resize event";
    if (scene_)
    {
        scene_->updateSceneRect();
    }
    QGraphicsView::resizeEvent(event);
}

void FlowView::dropEvent(QDropEvent* event)
{
    setFocus(Qt::OtherFocusReason);
    QGraphicsView::dropEvent(event);
}

Item::NodeType FlowView::getType(UniversalGraphicsItem* item) const
{
    Q_UNUSED(item);
    return Item::NodeType::OtherType;
}

void FlowView::mouseReleaseEvent(QMouseEvent* event)
{
    if (cutterMode_ && rubberBand_ && rubberBand_->isVisible())
    {
        auto rect = rubberBand_->rect();
        if (rect.size().isEmpty())
            return;
        auto pos = rubberBand_->pos();
        emit areaCutted(QRect {pos, rect.size()});
        rubberBand_->hide();
        return;
    }

    QGraphicsView::mouseReleaseEvent(event);
    if (nullptr != scene_)
    {
        if (dragMode() == QGraphicsView::DragMode::RubberBandDrag || dragMode() == QGraphicsView::DragMode::ScrollHandDrag)
        {
            setDragMode(QGraphicsView::NoDrag);
            scene_->setDragMode(FlowScene::DragMode::NoDrag);
        }
    }
}

int FlowView::getCurrentScale() const
{
    return currentScale_;
}

void FlowView::changeScaleForResizableItems(double scale)
{
    if (!scene_)
        return;

    auto viewMode = currentScale_ <= MINI_SCALE ? TitleItem::ViewMode::Underline : TitleItem::ViewMode::Normal;

    for (auto it : scene_->graphicsItems())
    {
        auto item = dynamic_cast<UniversalGraphicsItem*>(it.second);
        if (item)
        {
            item->setCurrentScale(currentScale_);
            item->setTitleViewMode(viewMode);
        }
    }
}

void FlowView::changeTextItemsVisibility(double scale)
{
    for (auto item : items())
    {
        if (auto it = dynamic_cast<QGraphicsTextItem*>(item))
        {
            if (scale <= 30)
            {
                if (it->isVisible())
                {
                    it->setVisible(false);
                }
            }
            else
            {
                if (!it->isVisible())
                {
                    it->setVisible(true);
                }
            }
        }
    }
}

void FlowView::setBackColor(const QColor &color)
{
    backColor_ = color;
}

QColor FlowView::getBackColor() const
{
    return backColor_;
}

void FlowView::showTopLevelConnections(bool flag) const
{
    if (!scene_)
    {
        return;
    }

    for (auto item : scene_->items())
    {
        auto projItem = dynamic_cast<UniversalPortProjection*>(item);
        if (projItem)
        {
            continue;
        }

        auto arrowControllers = scene_->arrowControllers();
        for (auto arrowController : arrowControllers)
        {
            auto destItem = arrowController->destination();
            auto srcItem = arrowController->source();
            if (destItem == projItem || srcItem == projItem)
            {
                projItem->setVisible(flag);
                arrowController->arrow()->setVisible(flag);
            }
        }
    }
}

void FlowView::hideEvent(QHideEvent* event)
{
    showTopLevelConnections();
    QGraphicsView::hideEvent(event);
}

// TODO SCALE REFACTORING test and remove this function
void FlowView::changeScaleForItemToolBar()
{
    if (!scene_)
        return;

    // TODO fix a lot of for
    auto items = scene_->graphicsItems();
    for (auto it : items)
    {
        if (auto item = dynamic_cast<UniversalGraphicsItem*>(it.second); item)
        {
            item->correctToolBarBrushGeometry(currentScale_);
        }
        else if (auto item = dynamic_cast<FrameGraphicsItem*>(it.second); item)
        {
            item->correctToolBarBrushSize(currentScale_);
        }
    }
}
} // namespace QtNodes
