#include "singletones/ClipboardController.hpp"
#include "graphicsComposites/universalGraphicsObject/UniversalGraphicsItem.h"
#include "models/NodesScene.hpp"
#include "scenes/FlowScene.hpp"
#include "singletones/Facade.hpp"
#include "views/FlowView.hpp"

#include <QAction>
#include <QMimeData>

using namespace QtNodes;

ClipboardController::ClipboardController()
: copyBuffer_ {nullptr}
, cutBuffer_ {}
{
    copyAction_ = new QAction {tr("Copy")};
    copyAction_->setShortcut(QKeySequence::Copy);

    cutAction_ = new QAction {tr("Cut")};
    cutAction_->setShortcut(QKeySequence::Cut);

    pasteAction_ = new QAction {tr("Paste")};
    pasteAction_->setShortcut(QKeySequence::Paste);

    connect(copyAction_, &QAction::triggered, this, &ClipboardController::copy);
    connect(cutAction_, &QAction::triggered, this, &ClipboardController::cut);
    connect(pasteAction_, &QAction::triggered, this, &ClipboardController::paste);
}

ClipboardController &ClipboardController::Instance()
{
    static ClipboardController clipboard;
    return clipboard;
}

QMimeData* ClipboardController::copyBuffer() const
{
    return copyBuffer_;
}

std::list<CutData> ClipboardController::cutBuffer() const
{
    return cutBuffer_;
}

QAction* ClipboardController::copyAction() const
{
    copyAction_->setText(tr("Copy"));
    return copyAction_;
}

QAction* ClipboardController::cutAction() const
{
    cutAction_->setText(tr("Cut"));
    return cutAction_;
}

QAction* ClipboardController::pasteAction() const
{
    pasteAction_->setText(tr("Paste"));
    return pasteAction_;
}

void ClipboardController::copy()
{
    if (copyBuffer_)
    {
        copyBuffer_->deleteLater();
    }
    if (!cutBuffer_.empty())
    {
        cutBuffer_.clear();
    }
    copyBuffer_ = Facade::Instance().copy();
}

void ClipboardController::cut()
{
    if (copyBuffer_)
    {
        copyBuffer_->deleteLater();
        copyBuffer_ = nullptr;
    }
    cutBuffer_ = Facade::Instance().cut();
}

void ClipboardController::paste()
{
    auto items = Facade::Instance().sceneModel()->selectedItems();
    QUuid targetScene;
    QtNodes::FlowScene* scene = nullptr;
    QPointF pos {};
    if (items.size() == 1)
    {
        auto grItem = dynamic_cast<QtNodes::UniversalGraphicsItem*>(items.first());
        if (nullptr != grItem)
        {
            scene = grItem->innerScene();
            if (scene)
            {
                targetScene = scene->id();
                if (!scene->views().empty())
                {
                    auto view = scene->views().at(0);

                    auto mapPos = view->viewport()->rect().topLeft();
                    pos = view->mapToScene(mapPos) + QPoint {20, 20};
                }
            }
        }
    }
    else
    {
        scene = QtNodes::Facade::Instance().sceneModel()->getTopScene();
        if (scene)
        {
            targetScene = scene->id();
            auto view = QtNodes::Facade::Instance().sceneModel()->getTopView();
            if (view)
            {
                auto mapPos = view->viewport()->rect().topLeft();
                pos = view->mapToScene(mapPos) + QPoint {20, 20};
            }
        }
    }

    if (copyBuffer_ || !cutBuffer_.empty())
    {
        Facade::Instance().paste(copyBuffer_, targetScene, pos);
    }
}
