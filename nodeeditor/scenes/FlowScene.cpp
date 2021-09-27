#include "FlowScene.hpp"

#include "graphicsComposites/universalGraphicsObject/ResizableItem/Corner.h"
#include "arrows/AbstractArrow.h"
#include "arrows/ConnectionArrow.h"
#include "arrows/CurvedArrow.h"
#include "arrows/LateralHorizontalArrow.h"
#include "arrows/LateralVerticalArrow.h"
#include "arrows/LineArrow.h"
#include "arrows/PolyLineArrow.h"
#include "arrows/TreeStyleHorizontalArrow.h"
#include "arrows/TreeStyleVerticalArrow.h"
#include "graphicsComposites/frame/FrameGraphicsItem.hpp"
#include "models/NodesModel.hpp"
#include "models/NodesScene.hpp"
#include "nodes/Connection.hpp"
#include "nodes/Node.hpp"
#include "nodes/UniversalGraphicsItemImpl.h"
#include "singletones/Facade.hpp"
#include "singletones/MenuBuilder.h"
#include "graphicsComposites/text/NoteGraphicsItem.hpp"
#include "graphicsComposites/text/TextGraphicsItem.hpp"
#include "views/FlowView.hpp"
#include "views/MiniView.hpp"
#include "common/ArrowController.h"
#include "nodes/ConnectionID.hpp"
#include "nodes/FlowItemMimeData.hpp"
#include "repository/repository/ComponentMimeData.hpp"
#include "nodes/NodeIndex.hpp"
#include "graphicsComposites/universalGraphicsObject/UniversalGraphicsItem.h"
#include "common/nodeeditor_globals.hpp"
#include "factories/CommandFactory.hpp"
#include "singletones/UndoController.hpp"
#include "utils/undo/AddGraphicsArrowCommand.hpp"
#include "utils/undo/AddGraphicsFrameCommand.hpp"
#include "utils/undo/AddGraphicsTextCommand.hpp"
#include "utils/undo/CommandStack.hpp"
#include "utils/undo/RemoveGraphicsArrowCommand.hpp"
#include "utils/undo/RemoveGraphicsFrameCommand.hpp"
#include "utils/undo/RemoveGraphicsTextCommand.hpp"
#include "arrows/TinyMath.h"
#include "src/widgets/GridAlignmentDialog.h"

#include <QApplication>
#include <QDebug>
#include <QGraphicsSceneContextMenuEvent>
#include <QJsonArray>
#include <QMenu>

using namespace QtNodes;
using namespace dsrepository;

bool operator>(const QRectF &lhv, const QRectF &rhv)
{
    return lhv.height() > rhv.height() || lhv.width() > rhv.width();
}

bool operator>=(const QRectF &lhv, const QRectF &rhv)
{
    return lhv.height() >= rhv.height() || lhv.width() >= rhv.width();
}

bool operator<(const QRectF &lhv, const QRectF &rhv)
{
    return lhv.height() >= rhv.height() || lhv.width() >= rhv.width();
}

FlowScene::FlowScene(QObject* parent)
: QGraphicsScene(parent)
, SerializableIdentityUnit {}
, model_ {nullptr}
, lastFoundedConnectionableItem_ {nullptr}
, isNeedToSpecialSelection_ {false}
, pressedItem_ {nullptr}
, newArrowType_ {QtNodes::Arrows::ArrowType::NoType}
, newArrow_ {nullptr}
, sourceConnectionableItem_ {nullptr}
, curState_ {State::Normal}
, type_ {Item::NodeType::OtherType}
, backColor_ {"#222033"}
, foundedUGIPortBarColor_ {"#409d84ff"}
, dragMode_ {DragMode::NoDrag}
, isSingleMouseButton_ {true}
{
    // Event for controll item selection
    connect(this, &FlowScene::selectionChanged, this, &FlowScene::selectionHasChanged);
}

FlowScene::FlowScene(Item::NodeType type, const QUuid &id, Item::DeploymentType deploymentType, QObject* parent)
: QGraphicsScene {parent}
, SerializableIdentityUnit {id}
, model_ {nullptr}
, lastFoundedConnectionableItem_ {nullptr}
, isNeedToSpecialSelection_ {false}
, pressedItem_ {nullptr}
, newArrowType_ {QtNodes::Arrows::ArrowType::NoType}
, newArrow_ {nullptr}
, sourceConnectionableItem_ {nullptr}
, curState_ {State::Normal}
, type_ {type}
, deploymentType_ {deploymentType}
, backColor_ {"#222033"}
, foundedUGIPortBarColor_ {"#9d84ff"}
, isSingleMouseButton_ {true}
{
    setSceneRect(0, 0, 0, 0);

    // NOTE
    // this line disable indexing. see QTBUG-18021
    setItemIndexMethod(QGraphicsScene::ItemIndexMethod::NoIndex);

    // Event for controll item selection
    connect(this, &FlowScene::selectionChanged, this, &FlowScene::selectionHasChanged);
}

FlowScene::~FlowScene()
{
    for (const auto &c : arrowControllers_)
    {
        disconnect(c, nullptr, nullptr, nullptr);
        c->deleteLater();
    }
    arrowControllers_.clear();
    // clear();
}

void FlowScene::changeArrowType(const QUuid &arrowId, const QtNodes::Arrows::ArrowType &arrowType)
{
    qDebug() << "FlowScene:" << this->id() << "changeArrowType";
    std::set<ArrowController*>::iterator it;
    for (it = arrowControllers_.begin(); it != arrowControllers_.end(); ++it)
    {
        if (nullptr != *it && (*it)->arrow()->id() == arrowId)
        {
            break;
        }
    }
    if (nullptr != *it)
    {
        ArrowController* arrowController = *it;
        Arrows::AbstractArrow* newArrow = arrowController->arrow()->toAnotherArrowType(arrowType);

        if (nullptr != newArrow)
        {
            auto oldArrow = arrowController->arrow();
            auto source = arrowController->source();
            auto dest = arrowController->destination();
            UndoController::Instance().begin("changeArrowType");
            deleteGraphicsItem(oldArrow);
            addArrowController(source, dest, newArrow);
            UndoController::Instance().end();
            newArrow->setSelected(true);
        }
    }
}

Item::NodeType FlowScene::type() const
{
    return type_;
}

Item::DeploymentType FlowScene::deploymentType() const
{
    return deploymentType_;
}

UniversalSelectableItem* FlowScene::addGraphicsFrame(QPointF pos, QSizeF size)
{
    UndoController::Instance().begin("FlowScene: addGraphicsFrame");

    auto command = UndoController::Instance().factory()->createAddGraphicsFrameCommand(id(), pos, size, {});
    UndoController::Instance().push(command);

    UndoController::Instance().end();

    return graphicsItem(command->value());
}

void FlowScene::addUniversalGraphicsItem(UniversalGraphicsItem* item, const QPointF &pos)
{
    addGraphicsItem(item, pos);
    lastUid_ = item->id();
}

bool FlowScene::addGraphicsItem(UniversalSelectableItem* item, const QPointF &pos)
{
    if (nullptr == item)
    {
        return false;
    }

    auto itemId = item->id();

    auto insertRet = graphicsItems_.insert_or_assign(itemId, item);

    if (!insertRet.second)
    {
        return false;
    }

    if (auto* oldScene = item->flowScene(); oldScene != nullptr && oldScene != this)
    {
        oldScene->removeGraphicsItem(itemId);
    }

    addItem(item);
    item->setPos(pos);

    auto connectionableItem = dynamic_cast<UniversalConnectionableItem*>(item);
    if (nullptr != connectionableItem)
    {
        connect(connectionableItem, &UniversalConnectionableItem::geometryChanged, [this, connectionableItem]()
        {
            auto rect = connectionableItem->boundingRect();
            auto itemRect = QRectF {connectionableItem->scenePos(), QSizeF {rect.width(), rect.height()}};
            if (!rect.contains(itemRect))
            {
                this->updateSceneRect();
            }
        });
    }

    auto sceneModel = Facade::Instance().sceneModel();
    if (nullptr == sceneModel)
    {
        return true;
    }

    if (auto view = sceneModel->getTopView())
    {
        view->changeScaleForItemToolBar();
        view->changeScaleForResizableItems(view->getCurrentScale());
    }

    return true;
}

UniversalSelectableItem* FlowScene::addGraphicsFrame(QPointF pos, QSizeF size, const QJsonObject &restore)
{

    auto frame = new FrameGraphicsItem {size, QUuid::createUuid()};

    if (!restore.isEmpty())
    {
        frame->restore(restore);
        pos = frame->scenePos();
    }

    if (addGraphicsItem(frame, pos))
    {
        Facade::Instance().connectBoundaryFrame(frame);

        if (auto view = Facade::Instance().sceneModel()->getTopView())
            view->changeScaleForItemToolBar();

        return frame;
    }
    else
    {
        frame->deleteLater();
        return nullptr;
    }
}

std::vector<ResizableItem*> FlowScene::getResizableItems() const
{
    std::vector<ResizableItem*> ret;
    for (auto item : items())
    {
        if (auto it = dynamic_cast<ResizableItem*>(item))
        {
            ret.push_back(it);
        }
    }
    return ret;
}

FlowView* FlowScene::flowView() const
{
    for (const auto &view : views())
    {
        if (auto flowView = dynamic_cast<FlowView*>(view); flowView)
        {
            return flowView;
        }
    }

    return nullptr;
}

void FlowScene::associateItemWith(const QUuid &id, const QUuid &parentId)
{
    if (!parentId.isNull())
    {
        if (auto association = associations_.find(parentId); association != associations_.end())
        {
            association->second.insert(id);
        }
        else
        {
            std::set<QUuid> set;
            set.insert(id);
            associations_.insert(std::make_pair(parentId, set));
        }
    }
    else
    {
        for (auto &association : associations_)
        {
            if (auto set = association.second; set.find(id) != set.end())
            {
                association.second.erase(id);
            }
        }
    }
}

std::set<QUuid> FlowScene::associatedItems(const QUuid &id)
{
    std::set<QUuid> ret;
    if (auto found = associations_.find(id); found != associations_.end())
    {
        return found->second;
    }
    return ret;
}

TextGraphicsItem* FlowScene::addTextItem(const QPointF &pos, const QJsonObject &restore, const QUuid &parentId)
{
    qDebug() << "FlowScene:" << this->id() << "addTextItem";
    auto scenePos = pos;
    auto textItem = new TextGraphicsItem(QUuid::createUuid());
    textItem->setParentItem(parentId);

    if (!restore.isEmpty())
    {
        textItem->restore(restore);
        scenePos = textItem->scenePos();
    }

    if (!parentId.isNull())
    {
        associateItemWith(textItem->id(), parentId);
    }

    if (addGraphicsItem(textItem, scenePos))
    {
        return textItem;
    }
    else
    {
        textItem->deleteLater();
        return nullptr;
    }
}

ArrowController* FlowScene::addArrowController(UniversalConnectionableItem* source, UniversalConnectionableItem* dest,
                                               Arrows::AbstractArrow* arrow, const QJsonObject &restore)
{
    if (!source || !dest)
    {
        return nullptr;
    }

    if (!arrow && !restore.isEmpty())
    {
        auto type = static_cast<Arrows::ArrowType>(restore["arrow"].toObject()["type"].toInt());
        arrow = createArrow(type);
    }
    else if (!arrow)
    {
        return nullptr;
    }

    auto newArrowController = new ArrowController(source, dest, arrow);
    auto foundedController = arrowControllers_.find(newArrowController);

    if (foundedController != arrowControllers_.end())
    {
        qWarning() << "FlowScene::addArrowController(UniversalConnectionableItem *source, UniversalConnectionableItem *dest, Arrows::AbstractArrow *arrow, "
                      "const QJsonObject &restore): arrowControllers_ does already contains this newArrowController!";
        return nullptr;
    }
    connect(arrow, &Arrows::AbstractArrow::arrowChangingType, this, &FlowScene::arrowChangingType);
    connect(newArrowController, &ArrowController::destroyMe, this, &FlowScene::destroyArrowController);

    if (arrow && !arrow->scene())
    {
        addItem(arrow);
    }

    if (!restore.isEmpty())
    {
        newArrowController->restore(restore);
    }
    newArrowController->update(Arrows::UpdateGeometryMode::UpdatedBothItems);
    arrowControllers_.insert(newArrowController);
    graphicsItems_.insert({arrow->id(), arrow});

    return newArrowController;
}

void FlowScene::deleteGraphicsItemCommand(UniversalSelectableItem* item)
{
    if (item)
    {
        deleteGraphicsItemCommand(item->id());
    }
}

void FlowScene::deleteGraphicsItemCommand(const QUuid &uid)
{
    UniversalSelectableItem* retval = nullptr;

    auto found = graphicsItems_.find(uid);
    if (found != graphicsItems_.end())
    {
        retval = found->second;
        graphicsItems_.erase(found);
        removeItem(retval);
        retval->deleteLater();
        associateItemWith(uid, {});
        for (auto id : associatedItems(uid))
        {
            deleteGraphicsItem(id);
        }
        associations_.erase(uid);
    }
}

void FlowScene::alignByGrid()
{
    auto alignItemsCount = selectedAlignableItems_.count();
    if (alignItemsCount == 0)
    {
        return;
    }

    GridAlignmentDialog dialog {alignItemsCount};
    if (dialog.exec() != QDialog::Accepted)
    {
        return;
    }

    auto rowCount = dialog.rowCount();
    auto columnCount = dialog.columnCount();

    QList<QList<ResizableItem*>> alignItems;
    for (auto i = 0; i < rowCount; i++)
    {
        QList<ResizableItem*> row;
        for (auto j = 0; j < columnCount; j++)
        {
            auto index = i * columnCount + j;
            auto item = (index < alignItemsCount) ? selectedAlignableItems_[index] : nullptr;
            row.append(item);
        }
        alignItems.append(row);
    }

    moveOnPositions(alignItems, rowCount, columnCount);
}

void FlowScene::moveOnPositions(const QList<QList<ResizableItem*>> &alignItems, qint32 rowCount, qint32 columnCount)
{
    auto startPos = alignItems.constFirst().constFirst()->pos();
    auto prevX = startPos.x();
    auto prevY = startPos.y();

    const auto spacing = 60.;

    for (auto columnIndex = 0; columnIndex < columnCount; columnIndex++)
    {
        auto width = 0;
        if (columnIndex >= 1)
        {
            QList<ResizableItem*> column;
            for (auto rowIndex = 0; rowIndex < rowCount; rowIndex++)
            {
                auto item = alignItems.at(rowIndex).at(columnIndex - 1);
                if (item)
                {
                    column.append(item);
                }
            }
            width = detectMaximumItemSize(column, true);
        }

        auto curSpacing = columnIndex == 0 ? 0 : spacing;

        const auto x = prevX + width + curSpacing;

        for (auto rowIndex = 0; rowIndex < rowCount; rowIndex++)
        {
            auto item = alignItems.at(rowIndex).at(columnIndex);
            if (item)
            {
                item->setX(x);
            }
        }

        prevX = x;
    }

    for (auto rowIndex = 0; rowIndex < rowCount; rowIndex++)
    {
        const auto height = rowIndex >= 1 ? detectMaximumItemSize(alignItems[rowIndex - 1], false) : 0;

        auto curSpacing = rowIndex == 0 ? 0 : spacing;

        const auto y = prevY + height + curSpacing;

        for (auto columnIndex = 0; columnIndex < columnCount; columnIndex++)
        {
            auto item = alignItems.at(rowIndex).at(columnIndex);
            if (item)
            {
                item->setY(y);
            }
        }

        prevY = y;
    }
}

qreal FlowScene::detectMaximumItemSize(const QList<ResizableItem*> &alignItems, bool isWidth) const
{
    auto maxLenIt = std::max_element(alignItems.begin(), alignItems.end(),
                                     [isWidth](ResizableItem* left, ResizableItem* right)
                                     {
                                         return isWidth ? (left->width() < right->width()) : (left->height() < right->height());
                                     });

    auto index = std::distance(alignItems.begin(), maxLenIt);
    auto item = alignItems.at(int(index));
    if (item)
    {
        return isWidth ? item->width() : item->height();
    }
    return 0;
}

TextGraphicsItem* FlowScene::addTextBoxNote(QPointF pos, QSizeF size, UniversalGraphicsItem* item)
{
    UndoController::Instance().begin("FlowScene::addTextBoxNote");
    auto note = dynamic_cast<TextGraphicsItem*>(addTextItem(pos, item->id()));
    if (note)
    {
        note->setSize(size);

        auto line = new Arrows::LineArrow();
        line->setProperty(Item::GraphicsItemProperties::ArrowDirection,
                          QVariant {static_cast<int>(Arrows::Direction::NoDirection)});
        auto arrowPen = line->pen();
        arrowPen.setStyle(Qt::DotLine);
        arrowPen.setColor("#AAAAC5");
        line->setPen(arrowPen);

        addArrowController(item, note, line);
    }
    UndoController::Instance().end();
    return note;
}

QtNodes::NoteGraphicsItem* FlowScene::addNoteItem(QPointF pos, QSizeF size, const QString &text,
                                                  UniversalGraphicsItem* item)
{
    auto note = new NoteGraphicsItem(QUuid::createUuid(), item->id(), size, text);
    auto insertRet = graphicsItems_.insert(std::make_pair(note->id(), note));

    if (insertRet.second)
    {
        addItem(note);
        note->setPos(pos);
        connect(note, &NoteGraphicsItem::geometryChanged, [this, note]()
        {
            auto rect = note->boundingRect();
            auto itemRect = QRectF {note->scenePos(), QSizeF {rect.width(), rect.height()}};
            if (!rect.contains(itemRect))
            {
                this->updateSceneRect();
            }
        });

        auto line = new Arrows::LineArrow();
        line->setProperty(Item::GraphicsItemProperties::ArrowDirection,
                          QVariant {static_cast<int>(Arrows::Direction::NoDirection)});
        auto arrowPen = line->pen();
        arrowPen.setStyle(Qt::DotLine);
        arrowPen.setColor("#AAAAC5");
        line->setPen(arrowPen);
        addItem(line);
        auto controller = new ArrowController(item, note, line);
        connect(controller, &ArrowController::destroyMe, this, &FlowScene::destroyArrowController);
        arrowControllers_.insert(controller);
        connectionLines_.insert(std::make_pair(item->id(), note->id()));

        return note;
    }
    return nullptr;
}

QSet<QGraphicsItem*> FlowScene::itemsInRect(const QRectF &rect) const
{
    QPainterPath path;
    path.addRect(rect);
    return itemsInPath(path);
}

QSet<QGraphicsItem*> FlowScene::itemsInPath(const QPainterPath &path) const
{
    if (path.isEmpty())
    {
        return {};
    }

    QSet<QGraphicsItem*> ret;
    for (QGraphicsItem* item : items())
    {
        if (nullptr == item)
        {
            continue;
        }
        bool isArrow = nullptr != dynamic_cast<Arrows::AbstractArrow*>(item);
        if (isArrow)
        {
            continue;
        }
        QPainterPath itemShape;
        itemShape = item->mapToScene(item->shape());
        if (itemShape.isEmpty())
        {
            continue;
        }

        if (path.contains(itemShape))
        {
            ret.insert(item);
        }
    }
    return ret;
}

QSet<UniversalConnectionableItem*> FlowScene::connectionableItemsInPath(const QPainterPath &path) const
{
    QSet<QGraphicsItem*> allItemsInRect = itemsInPath(path);
    QSet<UniversalConnectionableItem*> ret;
    UniversalConnectionableItem* connectionableItem {nullptr};
    for (auto itemInRect : allItemsInRect)
    {
        connectionableItem = dynamic_cast<UniversalConnectionableItem*>(itemInRect);
        if (nullptr != connectionableItem)
        {
            ret.insert(connectionableItem);
        }
    }
    return ret;
}

UniversalSelectableItem* FlowScene::graphicsItem(const QUuid &id) const
{
    if (auto i = graphicsItems_.find(id); i != graphicsItems_.end())
    {
        return graphicsItems_.at(id);
    }
    else
    {
        return nullptr;
    }
}

ArrowController* FlowScene::arrowController(const QUuid &arrowId) const
{
    auto found = std::find_if(arrowControllers_.begin(), arrowControllers_.end(), [arrowId](const auto &val)
    {
        if ((*val).arrow()->id() == arrowId)
        {
            return true;
        }
        return false;
    });

    if (found != arrowControllers_.end())
    {
        return *found;
    }

    return nullptr;
}

UniversalSelectableItem* FlowScene::removeGraphicsItem(const QUuid &uid)
{
    UniversalSelectableItem* retval = nullptr;

    auto found = graphicsItems_.find(uid);
    if (found != graphicsItems_.end())
    {
        retval = found->second;
        graphicsItems_.erase(found);
        removeItem(retval);
        associateItemWith(uid, {});

        for (auto id : associatedItems(uid))
        {
            removeGraphicsItem(id);
        }
        associations_.erase(uid);
    }

    return retval;
}

void FlowScene::removeGraphicsItem(UniversalSelectableItem* item)
{
    if (item)
        removeGraphicsItem(item->id());
}

void FlowScene::deleteGraphicsItem(UniversalSelectableItem* item)
{
    if (item)
        deleteGraphicsItem(item->id());
}

void FlowScene::deleteGraphicsItem(const QUuid &uid)
{
    auto item = dynamic_cast<UniversalSelectableItem*>(graphicsItem(uid));

    if (!item)
    {
        return;
    }

    auto type = static_cast<Item::NodeType>(item->type());

    AbstractCommand* command = nullptr;

    switch (type)
    {
        case Item::NodeType::Frame:
        {
            command = UndoController::Instance().factory()->createRemoveGraphicsFrameCommand(id(), item->id(), {});
            break;
        }
        case Item::NodeType::Text:
        {
            QUuid parentId;
            if (auto it = dynamic_cast<TextGraphicsItem*>(item))
            {
                parentId = it->getParentItem();
            }
            command = UndoController::Instance().factory()->createRemoveGraphicsTextCommand(id(), item->id(), parentId,
                                                                                            {});
            break;
        }
        default:
        {
            auto type = static_cast<Arrows::ArrowType>(item->type());
            if (Arrows::ArrowType::Connection != type)
            {
                command = UndoController::Instance().factory()->createRemoveGraphicsArrowCommand(id(), item->id(), {});
            }
            break;
        }
    }
    if (command)
    {
        UndoController::Instance().begin("FlowScene: deleteGraphicsItem");
        UndoController::Instance().push(command);

        UndoController::Instance().end();
    }
}

std::map<QUuid, UniversalSelectableItem*> FlowScene::graphicsItems() const
{
    qDebug() << "FlowScene:" << this->id() << "graphicsItems";
    return graphicsItems_;
}

void FlowScene::createLineConnection(const QUuid &outItem, const QUuid &outPort, const QUuid &inItem,
                                     const QUuid &inPort, const QUuid &communicationId)
{
    auto model = Facade::Instance().model();
    if (nullptr == model)
    {
        return;
    }

    auto connections = model->nodeConnections(model->nodeIndex(outItem));

    Arrows::ConnectionArrow* line = nullptr;

    auto found = std::find_if(connections.begin(), connections.end(), [&communicationId](const auto &val)
    {
        if (val->uuid() == communicationId)
        {
            return true;
        }
        return false;
    });
    if (found == connections.end())
    {
        return;
    }

    auto range = connectionLines_.equal_range(outPort);

    if (range.first != connectionLines_.end())
    {
        for (auto it = range.first; it != range.second && it != connectionLines_.end(); ++it)
        {
            if (const auto &pair = *it; pair.second == inPort)
            {
                addConnectionToPort(pair.first, communicationId);
                addConnectionToPort(pair.second, communicationId);

                for (auto arrowController : arrowControllers_)
                {
                    bool isBreak = false;
                    auto connectionArrow = dynamic_cast<Arrows::ConnectionArrow*>(arrowController->arrow());
                    if (nullptr != connectionArrow)
                    {
                        auto arrowsConnections = connectionArrow->connections();
                        for (auto arrowConnection : arrowsConnections)
                        {
                            if (arrowConnection->id().commId_ == found->get()->id().commId_)
                            {
                                line = connectionArrow;
                                line->addConnection(found->get(), true);
                                if (!line->isVisible())
                                {
                                    line->showConnection(found->get());
                                }
                                isBreak = true;
                                break;
                            }
                        }
                        if (isBreak)
                        {
                            break;
                        }
                    }
                }

                return;
            }
        }
    }

    auto from = dynamic_cast<UniversalGraphicsItem*>(graphicsItem(outItem))->getPort(outPort);
    auto to = dynamic_cast<UniversalGraphicsItem*>(graphicsItem(inItem))->getPort(inPort);

    line = new Arrows::ConnectionArrow({found->get()}, true);
    connect(found->get(), &Connection::updated, [=](Connection &connection)
    {
        for (const auto &controller : arrowControllers_)
        {
            if (auto arrow = dynamic_cast<Arrows::ConnectionArrow*>(controller->arrow());
            nullptr != arrow && arrow->id() == connection.uuid())
            {
                arrow->setDeviceType(connection.id().deviceType_);
                arrow->setDeviceName(connection.id().deviceName_);
            }
        }
    });

    line->setDeviceType(found->get()->id().deviceType_);

    auto devName = found->get()->id().deviceName_;
    line->setDeviceName(devName);

    line->setProperty(Item::GraphicsItemProperties::ArrowDirection,
                      QVariant {static_cast<int>(Arrows::Direction::NoDirection)});
    addArrowController(from, to, line, {});
    connectionLines_.insert(std::make_pair(outPort, inPort));

    addConnectionToPort(outPort, communicationId);
    addConnectionToPort(inPort, communicationId);
}

void FlowScene::createLineConnection(const QUuid &outItem, const QUuid &outPort,
                                     UniversalConnectionableItem* internalPort, const QUuid &communicationId)
{
    qDebug() << "FlowScene:" << this->id() << "createLineConnection";
    auto model = Facade::Instance().model();

    auto connections = model->nodeConnections(model->nodeIndex(outItem));

    auto found = std::find_if(connections.begin(), connections.end(), [&communicationId](const auto &val)
    {
        if (val->uuid() == communicationId)
        {
            return true;
        }
        return false;
    });
    if (found == connections.end())
    {
        qCritical() << "Fatal! Comm uuid not found";
        return;
    }

    auto range = connectionLines_.equal_range(outPort);
    if (range.first != connectionLines_.end())
    {
        for (auto it = range.first; it != range.second && it != connectionLines_.end(); ++it)
        {
            if (const auto &pair = *it; pair.second == internalPort->id())
            {
                addConnectionToPort(pair.first, communicationId);

                Arrows::ConnectionArrow* line = nullptr;

                for (auto arrowController : arrowControllers_)
                {
                    bool isBreak = false;
                    auto connectionArrow = dynamic_cast<Arrows::ConnectionArrow*>(arrowController->arrow());
                    if (nullptr != connectionArrow)
                    {
                        auto arrowConnections = connectionArrow->connections();
                        for (auto arrowConnection : arrowConnections)
                        {
                            if (arrowConnection->id().commId_ == found->get()->id().commId_)
                            {
                                line = connectionArrow;
                                line->addConnection(found->get(), true);

                                if (!line->isVisible())
                                {
                                    line->showConnection(found->get());
                                }

                                isBreak = true;
                                break;
                            }
                        }
                        if (isBreak)
                        {
                            break;
                        }
                    }
                }
                return;
            }
        }
    }

    auto from = dynamic_cast<UniversalGraphicsItem*>(graphicsItem(outItem))->getPort(outPort);
    auto to = internalPort;

    if (nullptr == model)
    {
        qCritical() << "Fatal! model is nullptr";
        return;
    }

    Arrows::ConnectionArrow* line = nullptr;
    line = new Arrows::ConnectionArrow({found->get()}, false);
    line->setProperty(Item::GraphicsItemProperties::ArrowDirection,
                      QVariant {static_cast<int>(Arrows::Direction::NoDirection)});
    addArrowController(from, to, line, {});
    connectionLines_.insert(std::make_pair(from->id(), to->id()));
    addConnectionToPort(outPort, communicationId);
}

void FlowScene::removeLineConnection(const QUuid &itemIdx, const QUuid &communicationId)
{
    qDebug() << "FlowScene:" << this->id() << "removeLineConnection";
    if (auto item = dynamic_cast<UniversalGraphicsItem*>(graphicsItem(itemIdx)); item)
    {
        for (auto &port : ports_)
        {
            if (auto set = port.second; set.find(communicationId) != set.end())
            {
                auto portId = port.first;
                if (item->getPort(portId))
                {
                    port.second.erase(communicationId);
                    if (port.second.empty())
                    {
                        item->removePort(portId);
                        removePort(portId);
                        return;
                    }
                }
            }
        }
    }
}

void FlowScene::addPort(const QUuid &id)
{
    qDebug() << "FlowScene:" << this->id() << "addPort";
    std::set<QUuid> set;
    ports_.insert(std::make_pair(id, set));
}

void FlowScene::setDragMode(const FlowScene::DragMode &mode)
{
    dragMode_ = mode;
}

FlowScene::DragMode FlowScene::dragMode() const
{
    return dragMode_;
}

void FlowScene::paste(const QJsonObject &node)
{
    QJsonObject tmp = node["pos"].toObject();
    QPointF pos = {tmp["x"].toDouble(), tmp["y"].toDouble()};

    tmp = node["size"].toObject();
    QSizeF size = {tmp["width"].toDouble(), tmp["height"].toDouble()};

    auto type = static_cast<Item::NodeType>(node["type"].toInt());
    switch (type)
    {
        case Item::NodeType::Frame:
        {
            addGraphicsFrame(pos, size);
            break;
        }
        case Item::NodeType::Text:
        {
            addTextItem(pos);
            break;
        }
        default:
        {
            break;
        }
    }
}

void FlowScene::removePort(const QUuid &portId)
{
    qDebug() << "FlowScene:" << this->id() << "removePort";
    ports_.erase(portId);
}

UniversalConnectionableItem* FlowScene::findConnectionableItem(const QPointF &scenePos) const
{
    UniversalConnectionableItem* foundedConnectionableItem = nullptr;
    foundedConnectionableItem = findUniversalGraphicsItem(scenePos);
    if (nullptr == foundedConnectionableItem)
    {
        foundedConnectionableItem = findBoudary(scenePos);
    }
    return foundedConnectionableItem;
}

FrameGraphicsItem* FlowScene::findBoudary(const QPointF &scenePos) const
{
    FrameGraphicsItem* foundedFrame = nullptr;
    auto foundedItems = items(scenePos);
    for (auto item : foundedItems)
    {
        foundedFrame = dynamic_cast<FrameGraphicsItem*>(item);
        if (nullptr != foundedFrame)
            break;
    }
    return foundedFrame;
}

UniversalGraphicsItem* FlowScene::findUniversalGraphicsItem(const QPointF &scenePos) const
{
    UniversalGraphicsItem* foundedUGI = nullptr;
    auto foundedItems = items(scenePos);
    for (auto item : foundedItems)
    {
        foundedUGI = dynamic_cast<UniversalGraphicsItem*>(item);
        if (nullptr != foundedUGI)
            break;
    }
    return foundedUGI;
}

Arrows::AbstractArrow* FlowScene::createArrow(Arrows::ArrowType type) const
{
    switch (type)
    {
        case Arrows::ArrowType::Line:
        {
            return new Arrows::LineArrow {};
        }
        case Arrows::ArrowType::Curved:
        {
            return new Arrows::CurvedArrow();
        }
        case Arrows::ArrowType::PolyLine:
        {
            return new Arrows::PolyLineArrow {};
        }
        case Arrows::ArrowType::LateralHorizontal:
        {
            return new Arrows::LateralHorizontalArrow {};
        }
        case Arrows::ArrowType::LateralVertical:
        {
            return new Arrows::LateralVerticalArrow {};
        }
        case Arrows::ArrowType::TreeStyleHorizontal:
        {
            return new Arrows::TreeStyleHorizontalArrow {};
        }
        case Arrows::ArrowType::TreeStyleVertical:
        {
            return new Arrows::TreeStyleVerticalArrow {};
        }
        default:
        {
            return nullptr;
        }
    }
}

void FlowScene::clear()
{
    for (const auto &[id, item] : graphicsItems_)
    {
        deleteGraphicsItem(id);
    }
}

bool FlowScene::addConnectionToPort(const QUuid &portId, const QUuid &connId)
{
    qDebug() << "FlowScene:" << this->id() << "addConnectionToPort";
    if (auto port = ports_.find(portId); port != ports_.end())
    {
        auto insertRet = port->second.insert(connId);
        return insertRet.second;
    }
    return false;
}

void FlowScene::removeConnectionFromPort(const QUuid &portId, const QUuid &connId)
{
    if (auto port = ports_.find(portId); port != ports_.end())
    {
        port->second.erase(connId);
    }
}

void FlowScene::setState(const State &state)
{
    if (state == curState_)
    {
        return;
    }
    if (curState_ == State::DrawArrow && state != State::AfterDrawArrow)
    {
        setState(State::AfterDrawArrow);
    }

    switch (state)
    {
        case State::Normal:
        {
            QApplication::restoreOverrideCursor();
            QApplication::setOverrideCursor(Qt::ArrowCursor);
            break;
        }
        case State::AddArrow:
        {
            QApplication::restoreOverrideCursor();
            QApplication::setOverrideCursor(Qt::PointingHandCursor);
            break;
        }
        case State::DrawArrow:
        {
            QApplication::restoreOverrideCursor();
            if (curState_ != State::AddArrow)
            {
                break;
            }
            QApplication::setOverrideCursor(Qt::CrossCursor);
            if (nullptr != newArrow_)
            {
                newArrow_->setDrawingMode();
            }
            auto sourceUniversalItem = dynamic_cast<UniversalGraphicsItem*>(sourceConnectionableItem_);
            if (nullptr != sourceUniversalItem)
            {
                sourceUniversalItem->setPortBarColor(foundedUGIPortBarColor_);
            }
            break;
        }
        case State::AfterDrawArrow:
        {
            if (curState_ != State::DrawArrow)
            {
                break;
            }
            auto sourceUniversalItem = dynamic_cast<UniversalGraphicsItem*>(sourceConnectionableItem_);
            if (nullptr != sourceUniversalItem)
            {
                sourceUniversalItem->setPortBarColorToDefault();
                sourceUniversalItem->update();
            }
            sourceConnectionableItem_ = nullptr;

            auto lastFoundedGraphicsItem = dynamic_cast<UniversalGraphicsItem*>(lastFoundedConnectionableItem_);
            if (nullptr != lastFoundedGraphicsItem)
            {
                lastFoundedGraphicsItem->setPortBarColorToDefault();
                lastFoundedGraphicsItem->update();
            }
            lastFoundedConnectionableItem_ = nullptr;

            if (nullptr != newArrow_)
            {
                newArrow_->resetDrawingMode();
                newArrow_ = nullptr;
            }

            curState_ = state;
            emit stateChanged(state);

            setState(State::Normal);
            return;
        }
        default:
        {
            break;
        }
    }

    curState_ = state;
    emit stateChanged(state);
}

FlowScene::State FlowScene::state() const
{
    qDebug() << "FlowScene:" << this->id() << "state";
    return curState_;
}

void FlowScene::setNewArrowType(const QtNodes::Arrows::ArrowType &type)
{
    qDebug() << "FlowScene:" << this->id() << "setNewArrowType";
    newArrowType_ = type;
}

Arrows::ArrowType FlowScene::newArrowType() const
{
    qDebug() << "FlowScene:" << this->id() << "newArrowType";
    return newArrowType_;
}

void FlowScene::setFlowModel(DataFlowModel* model)
{
    qDebug() << "FlowScene:" << this->id() << "setFlowModel";
    if (model_)
    {
        disconnect(this, nullptr, model_, nullptr);
        disconnect(model_, nullptr, this, nullptr);
    }

    model_ = model;

    if (model_)
    {
        connect(model, &NodesModel::destroyed, this, [this]()
        {
            model_ = nullptr;
        });
    }
}

DataFlowModel* FlowScene::nodesModel() const
{
    return model_;
}

void FlowScene::destroyArrowController()
{
    auto arrowController = qobject_cast<ArrowController*>(sender());
    if (nullptr == arrowController)
    {
        return;
    }
    auto foundedController = arrowControllers_.find(arrowController);
    if (foundedController == arrowControllers_.end())
    {
        return;
    }

    arrowControllers_.erase(foundedController);
    auto arrow = arrowController->arrow();
    arrowController->deleteLater();
    arrowController = nullptr;
    if (nullptr != arrow)
    {
        deleteGraphicsItemCommand(arrow);
    }
}

void FlowScene::createNewArrow(const QPointF &scenePos)
{
    switch (newArrowType_)
    {
        case QtNodes::Arrows::ArrowType::Line:
        {
            newArrow_ = new Arrows::LineArrow(scenePos, scenePos);
            break;
        }
        case QtNodes::Arrows::ArrowType::PolyLine:
        {
            newArrow_ = new Arrows::PolyLineArrow(scenePos, scenePos);
            break;
        }
        case QtNodes::Arrows::ArrowType::Curved:
        {
            newArrow_ = new Arrows::CurvedArrow(scenePos, scenePos);
            break;
        }
        default:
        {
            break;
        }
    }
}

void FlowScene::deleteNewArrow()
{
    if (nullptr != newArrow_)
    {
        removeItem(newArrow_);
        newArrow_->deleteLater();
        newArrow_ = nullptr;
    }
}

void FlowScene::returnMouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsScene::mouseReleaseEvent(event);
    checkSelectionsAfterReleaseEvent();
    pressedItem_ = nullptr;
}

void FlowScene::cleanUpAfterWrongArrowDrawing()
{
    deleteNewArrow();
    setState(State::AfterDrawArrow);
}

void FlowScene::clearAll(const bool &withChild)
{
    qDebug() << "FlowScene:" << this->id() << "clearAll";

    if (withChild)
    {
        auto childs = model_->nodeChildsInLowestLevel(model_->nodeIndex(id()));

        for (const auto &child : childs)
        {
            auto scene = Facade::Instance().sceneModel()->getScene(child.id());

            if (scene)
            {
                scene->clearAll(false);
            }
        }
    }
    else
    {
        for (const auto &[id, item] : graphicsItems_)
        {
            if (!dynamic_cast<UniversalGraphicsItem*>(item))
            {
                deleteGraphicsItem(id);
            }
        }
    }
}

QMenu* FlowScene::createContextMenu(QPointF pos)
{
    qDebug() << "FlowScene:" << this->id() << "createContextMenu";
    auto sceneMenu = MenuBuilder::sceneMenu(this, pos);

    return sceneMenu;
}

void FlowScene::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    if (!isSingleMouseButton_)
    {
        return;
    }

    QGraphicsScene::contextMenuEvent(event);

    auto currentPos = event->scenePos();
    auto sceneModel = Facade::Instance().sceneModel();
    if (nullptr != sceneModel)
    {
        auto topScene = sceneModel->getTopScene();
        if (!event->isAccepted() && topScene == this)
        {
            auto sceneMenu = createContextMenu(currentPos);
            auto actions = sceneMenu->actions();
            if (nullptr != sceneMenu)
            {
                sceneMenu->setFixedWidth(300);

                // Set shadow effect
                MenuBuilder::setMenuEffect(sceneMenu);

                auto cursorPos = QCursor::pos();
                sceneMenu->exec(cursorPos);
                sceneMenu->deleteLater();
            }
        }
    }
    lastPos_ = currentPos;
}

QList<ResizableItem*> FlowScene::selectedAlignableItems() const
{
    return selectedAlignableItems_;
}

UniversalGraphicsItem* FlowScene::graphicsItemAt(const QPointF &pos) const
{
    auto graphicsItems = items(pos);

    for (auto graphicsItem : graphicsItems)
        if (auto ugi = dynamic_cast<UniversalGraphicsItem*>(graphicsItem))
            return ugi;

    return nullptr;
}

void FlowScene::deleteSelectedNodes()
{
    auto selected = selectedItems();

    if (!selected.empty())
    {
        UndoController::Instance().begin("Remove node begin macro");

        for (QGraphicsItem* item : selected)
        {
            auto tmp = dynamic_cast<UniversalSelectableItem*>(item);
            if (!tmp)
            {
                continue;
            }

            switch (static_cast<Item::NodeType>(tmp->type()))
            {
                case Item::NodeType::SystemNode:
                case Item::NodeType::SubSystemNode:
                case Item::NodeType::ComputerNode:
                case Item::NodeType::ApplicationNode:
                case Item::NodeType::ComponentNode:
                case Item::NodeType::ExternalNode:
                {
                    auto ugi = dynamic_cast<UniversalGraphicsItem*>(item);
                    if (nullptr != ugi)
                    {
                        const auto ugiInnerScene = ugi->innerScene();
                        if (nullptr != ugiInnerScene)
                        {
                            ugiInnerScene->clearAll();
                        }
                    }

                    for (const auto &c : arrowControllers_)
                    {
                        if (c->source() == item || c->destination() == item)
                        {
                            deleteGraphicsItem(c->arrow());
                        }
                    }

                    qApp->processEvents();

                    Facade::Instance().removeComposite(tmp->id());
                    break;
                }
                default:
                {
                    deleteGraphicsItem(tmp->id());
                    break;
                }
            }
        }
        UndoController::Instance().end();
    }
}

void FlowScene::arrowChangingType(const QtNodes::Arrows::ArrowType &newArrowType)
{
    Arrows::AbstractArrow* arrow = qobject_cast<Arrows::AbstractArrow*>(sender());
    if (nullptr != arrow)
    {
        changeArrowType(arrow->id(), newArrowType);
    }
}

UniversalSelectableItem* FlowScene::addTextItem(const QPointF &pos, const QUuid &parentId)
{
    UndoController::Instance().begin("FlowScene: addTextItem");

    auto command = UndoController::Instance().factory()->createAddGraphicsTextCommand(id(), pos, parentId, {});
    UndoController::Instance().push(command);

    UndoController::Instance().end();

    return graphicsItem(command->value());
}

ArrowController* FlowScene::addArrowController(UniversalConnectionableItem* source, UniversalConnectionableItem* dest,
                                               Arrows::AbstractArrow* arrow)
{
    UndoController::Instance().begin("FlowScene: addArrowController");

    auto command = UndoController::Instance().factory()->createAddGraphicsArrowCommand(id(), source->id(), dest->id(),
                                                                                       arrow, {});
    UndoController::Instance().push(command);

    UndoController::Instance().end();

    return dynamic_cast<AddGraphicsArrowCommand*>(command)->arrowController();
}

void FlowScene::dragEnterEvent(QGraphicsSceneDragDropEvent* event)
{
    QGraphicsScene::dragEnterEvent(event);
}

void FlowScene::dragMoveEvent(QGraphicsSceneDragDropEvent* event)
{
    if (nullptr == event)
    {
        return;
    }

    const QMimeData* eventMimeData = event->mimeData();
    if (nullptr == eventMimeData || (!canDropMimeData(event->scenePos(), eventMimeData) && !eventMimeData->hasFormat(
    "TextItem") && !eventMimeData->hasFormat("Frame")))
    {
        return QGraphicsScene::dragMoveEvent(event);
    }
    event->accept();
}

void FlowScene::dragLeaveEvent(QGraphicsSceneDragDropEvent* event)
{
    QGraphicsScene::dragLeaveEvent(event);
}

void FlowScene::dropEvent(QGraphicsSceneDragDropEvent* event)
{
    if (nullptr == event)
    {
        return;
    }

    const QPointF &eventScenePos = event->scenePos();
    const QMimeData* eventMimeData = event->mimeData();

    // TODO
    if (nullptr == eventMimeData || !canDropMimeData(eventScenePos, eventMimeData) || (!eventMimeData->hasFormat(
    FlowItemMimeData::mimeType()) && !eventMimeData->hasFormat(ComponentMimeData::mimeType())))
    {
        return QGraphicsScene::dropEvent(event);
    }

    if (eventMimeData->hasFormat(ComponentMimeData::mimeType()))
    {
        const auto flowData = reinterpret_cast<const ComponentMimeData*>(eventMimeData);
        const auto deployType = (this->type() == Item::NodeType::MainScene) ? Item::DeploymentType::External : deploymentType_;
        const auto &components = flowData->components();
        for (const auto &component : components)
        {
            auto info = component.info_;
            auto path = component.path_;
            qDebug() << "COMPONENT PATH: " << path;
            QUuid source = info.take("id").toVariant().toUuid();

            if (!info.isEmpty())
            {
                emit nodeDropped(eventScenePos, source, id(), Item::NodeType::ComponentNode, deployType, info, path);
            }
        }
        return QGraphicsScene::dropEvent(event);
    }

    const auto flowData = reinterpret_cast<const FlowItemMimeData*>(eventMimeData);
    const QString &impName = flowData->impName();
    const Item::NodeType nodeType = Node::nodeTypeFromString(impName);

    if (nodeType == Item::NodeType::OtherType)
    {
        if (impName == "FrameNode")
        {
            addGraphicsFrame(eventScenePos, QSizeF {250, 250});
        }
        else if (impName == "TextNode")
        {
            addTextItem(eventScenePos);
        }
        return QGraphicsScene::dropEvent(event);
    }

    auto deployType = Item::DeploymentType::None;
    if (nodeType == Item::NodeType::SystemNode)
    {
        deployType = MenuBuilder::createDeployTypeMenu();
    }
    else if (type_ == Item::NodeType::MainScene)
    {
        deployType = Item::DeploymentType::External;
    }
    else
    {
        deployType = deploymentType_;
    }

    // cannot drop system with no type
    if (nodeType == Item::NodeType::SystemNode && deployType == Item::DeploymentType::None)
    {
        return QGraphicsScene::dropEvent(event);
    }

    const QJsonObject &info = flowData->info();
    const QUuid &source = info.value("id").toVariant().toUuid();
    emit nodeDropped(eventScenePos, source, id(), nodeType, deployType, info["implementation"].toObject(), "");

    QGraphicsScene::dropEvent(event);
}

bool FlowScene::canDropMimeData(const QPointF pos, const QMimeData* data)
{
    bool allowed {false};
    for (const auto &type : mimeTypes())
    {
        if (data->hasFormat(type))
        {
            allowed = true;
            break;
        }
    }
    if (!allowed)
    {
        return false;
    }

    QString impName;
    QUuid id;
    Item::DeploymentType deployType {Item::DeploymentType::Internal};
    if (data->hasFormat(ComponentMimeData::mimeType()))
    {
        impName = "ComponentNode";
    }
    else if (data->hasFormat(FlowItemMimeData::mimeType()))
    {
        auto flowData = reinterpret_cast<const FlowItemMimeData*>(data);
        id = flowData->info().take("id").toVariant().toUuid();
        impName = flowData->impName();
    }
    else
    {
        return false;
    }

    if (auto item = graphicsItem(id); item != nullptr)
    {
        return false;
    }
    else
    {
        auto deployType = model_->nodeDeploymentType(model_->nodeIndex(id));
        if (deployType == Item::DeploymentType::Internal && (deploymentType_ == Item::DeploymentType::External || deploymentType_ == Item::DeploymentType::Main))
        {
            return false;
        }
    }

    for (auto item : items(pos))
    {
        if (dynamic_cast<UniversalGraphicsItem*>(item) != nullptr)
        {
            return false;
        }
    }
    if (impName == "TextNode" || impName == "FrameNode")
    {
        return true;
    }
    if ((impName == "SystemNode") && (type() != Item::NodeType::MainScene))
    {
        return false;
    }
    if (deploymentType_ == Item::DeploymentType::External || type() == Item::NodeType::MainScene)
    {
        return true;
    }

    switch (type())
    {
        case Item::NodeType::SystemNode:
        {
            if (impName == "SubsystemNode" || impName == "DeviceNode")
            {
                return true;
            }
            break;
        }
        case Item::NodeType::SubSystemNode:
        {
            if (impName == "DeviceNode")
            {
                return true;
            }
            break;
        }
        case Item::NodeType::ComputerNode:
        {
            if (impName == "ApplicationNode")
            {
                return true;
            }
            break;
        }
        case Item::NodeType::ApplicationNode:
        {
            if (impName == "ComponentNode")
            {
                return true;
            }
            break;
        }
        case Item::NodeType::ComponentNode:
        {
            break;
        }
        default:
        {
            break;
        }
    }
    return false;
}

QStringList FlowScene::mimeTypes() const
{
    return {FlowItemMimeData::mimeType(), ComponentMimeData::mimeType()};
}

QJsonObject FlowScene::save() const
{
    qDebug() << "FlowScene:" << this->id() << "save";
    QJsonObject retval = SerializableIdentityUnit::save();

    QJsonObject obj = {
    {  "x"     , sceneRect().x()}
    , {"y"     , sceneRect().y()}
    , {"width" , sceneRect().width()}
    , {"height", sceneRect().height()}
    ,
    };

    retval["sceneRect"] = obj;
    retval["type"] = static_cast<int>(type_);
    retval["deploymentType"] = static_cast<int>(deploymentType_);

    QJsonArray arr;
    QJsonArray arrFrame;
    QJsonArray arrNotes;
    QJsonArray arrText;

    for (const auto &item : items(Qt::AscendingOrder))
    {
        if (auto i = dynamic_cast<UniversalGraphicsItem*>(item))
        {
            arr.push_back(i->save());
        }
        if (auto i = dynamic_cast<FrameGraphicsItem*>(item))
        {
            arrFrame.push_back(i->save());
        }
        if (auto i = dynamic_cast<NoteGraphicsItem*>(item))
        {
            arrNotes.push_back(i->save());
        }
        if (auto i = dynamic_cast<TextGraphicsItem*>(item))
        {
            arrText.push_back(i->save());
        }
    }

    if (type_ == Item::NodeType::MainScene)
    {
        auto itemRect = itemsBoundingRect();
        qDebug() << itemRect;
        qDebug() << views().at(0)->horizontalScrollBar()->value();
        qDebug() << views().at(0)->verticalScrollBar()->value();
        obj = {
        {  "x", itemRect.topLeft().x()}
        , {"y", itemRect.topLeft().y()}
        };
    }
    retval["pos"] = obj;

    retval["nodes"] = arr;
    retval["frames"] = arrFrame;
    retval["notes"] = arrNotes;
    retval["texts"] = arrText;
    arr = {};

    for (const auto &c : arrowControllers_)
    {
        arr.push_back(c->save());
    }
    retval["arrowControllers"] = arr;
    arr = {};

    for (const auto &[in, out] : ports_)
    {
        QJsonObject tmp = {{"in's", in.toString()}};
        QJsonArray tArr;
        for (const auto &j : out)
        {
            QJsonObject tmpJ = {{"out", j.toString()}};
            tArr.push_back(tmpJ);
        }
        tmp["out's"] = tArr;
        arr.push_back(tmp);
    }
    retval["ports"] = arr;
    arr = {};

    return retval;
}

void FlowScene::restore(const QJsonObject &obj)
{
    SerializableIdentityUnit::restore(obj);
    QJsonObject tmp = obj["sceneRect"].toObject();
    setSceneRect(tmp["x"].toDouble(), tmp["y"].toDouble(), tmp["width"].toDouble(), tmp["height"].toDouble());
    type_ = static_cast<Item::NodeType>(obj["type"].toInt());
    deploymentType_ = static_cast<Item::DeploymentType>(obj["deploymentType"].toInt());
    QJsonArray arr = obj["nodes"].toArray();

    if (type_ == Item::NodeType::MainScene)
    {
        tmp = obj["pos"].toObject();
        barPos_ = {tmp["x"].toDouble(), tmp["y"].toDouble()};
    }

    for (auto i : arr)
    {
        auto id = QUuid::fromString(i.toObject()["id"].toString());
        auto index = Facade::Instance().model()->nodeIndex(id);
        auto sceneId = QUuid::fromString(i.toObject()["sceneId"].toString());
        // TODO fix coef

        auto item = dynamic_cast<UniversalGraphicsItemImpl*>(Facade::Instance().model()->nodePointer(index)->imp());

        auto inner = Facade::Instance().scenePart(sceneId, 2).get();

        auto itemGr = new UniversalGraphicsItem(id, item, inner);
        Facade::Instance().connectComposite(itemGr);
        addUniversalGraphicsItem(itemGr, {1, 1});

        itemGr->restore(i.toObject());
        itemGr->updateWidget();
    }

    arr = obj["ports"].toArray();
    for (auto i : arr)
    {
        std::set<QUuid> set;
        QJsonArray iArr = i.toObject()["out's"].toArray();
        for (auto j : iArr)
        {
            set.insert(QUuid::fromString(j.toObject()["out"].toString()));
        }
        ports_.insert({QUuid::fromString(i.toObject()["in's"].toString()), set});
    }

    arr = obj["frames"].toArray();
    for (auto i : arr)
    {
        QJsonObject pos = i.toObject()["pos"].toObject();
        QJsonObject size = i.toObject()["size"].toObject();

        addGraphicsFrame({pos["x"].toDouble(), pos["y"].toDouble()},
                         {size["width"].toDouble(), size["height"].toDouble()}, i.toObject());
    }

    arr = obj["notes"].toArray();
    for (auto i : arr)
    {
        QJsonObject restoringPos = i.toObject()["pos"].toObject();
        QJsonObject restoringSize = i.toObject()["size"].toObject();
        auto parentId = QUuid(i.toObject()["parent"].toString());
        auto text = i.toObject()["text"].toString();

        if (auto it = graphicsItems_.find(parentId); it != graphicsItems_.end())
        {
            QPointF point {restoringPos["x"].toDouble(), restoringPos["y"].toDouble()};
            QSizeF size {restoringSize["width"].toDouble(), restoringSize["height"].toDouble()};
            addNoteItem(point, size, text, dynamic_cast<UniversalGraphicsItem*>(it->second));
        }
    }

    arr = obj["texts"].toArray();
    for (auto i : arr)
    {
        QJsonObject restoringPos = i.toObject()["pos"].toObject();
        QPointF point {restoringPos["x"].toDouble(), restoringPos["y"].toDouble()};
        addTextItem(point, i.toObject(), {i.toObject()["parent"].toString()});
    }
}

void FlowScene::restoreConnections(const QJsonObject &p)
{
    auto arr = p["arrowControllers"].toArray();

    for (auto i : arr)
    {
        auto lId = QUuid::fromString(i.toObject()["source"].toObject()["id"].toString());
        auto rId = QUuid::fromString(i.toObject()["dest"].toObject()["id"].toString());
        auto found = std::find_if(arrowControllers_.begin(), arrowControllers_.end(), [=](const auto &val)
        {
            if (val->source()->id() == lId && val->destination()->id() == rId)
            {
                return true;
            }
            return false;
        });

        auto line = i.toObject()["arrow"].toObject();
        auto type = static_cast<QtNodes::Arrows::ArrowType>(line["type"].toInt());
        if (arrowControllers_.end() != found && QtNodes::Arrows::ArrowType::Connection == type)
        {
            (*found)->restore(i.toObject());
            completeUpdateItemId((*found)->arrow());
            continue;
        }
        UniversalConnectionableItem* left {nullptr};
        UniversalConnectionableItem* right {nullptr};
        auto currentItems = items();
        for (auto item : currentItems)
        {
            auto d = dynamic_cast<UniversalConnectionableItem*>(item);
            if (nullptr == d)
            {
                continue;
            }

            if (d->id() == lId)
            {
                left = d;
                if (right)
                    break;
            }
            else if (d->id() == rId)
            {
                right = d;
                if (left)
                    break;
            }
        }
        if (nullptr == right || nullptr == left)
        {
            continue;
        }
        Arrows::AbstractArrow* arrow {nullptr};
        arrow = createArrow(type);
        if (nullptr == arrow)
        {
            continue;
        }
        addItem(arrow);
        arrow->restore(i.toObject()["arrow"].toObject());
        auto arrowController = new ArrowController(left, right, arrow);

        connect(arrowController, &ArrowController::destroyMe, this, &FlowScene::destroyArrowController);
        connect(arrow, &Arrows::AbstractArrow::arrowChangingType, this, &FlowScene::arrowChangingType);

        arrowController->restore(i.toObject());
        arrowControllers_.insert(arrowController);
        graphicsItems_.insert({arrow->id(), arrow});
    }
}

void FlowScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (!event)
    {
        return;
    }

    isSingleMouseButton_ = event->buttons() != (Qt::LeftButton | Qt::RightButton);
    if (!isSingleMouseButton_)
    {
        return;
    }

    lastPos_ = event->scenePos();

    switch (curState_)
    {
        case State::AddFrame:
        {
            setState(State::Normal);
            event->accept();
            break;
        }
        case State::AddArrow:
        {
            if (event->button() != Qt::LeftButton)
            {
                event->accept();
                return;
            }
            sourceConnectionableItem_ = findConnectionableItem(lastPos_);
            lastFoundedConnectionableItem_ = sourceConnectionableItem_;
            if (nullptr == sourceConnectionableItem_)
            {
                setState(State::Normal);
                event->accept();
                return;
            }
            createNewArrow(event->scenePos());
            if (nullptr == newArrow_)
            {
                setState(State::Normal);
                event->accept();
                return;
            }

            addItem(newArrow_);
            setState(State::DrawArrow);

            event->accept();
            return;
        }
        default:
        {
            break;
        }
    }

    checkSelectionsBeforePressEvent(event);
    QGraphicsScene::mousePressEvent(event);
}

void FlowScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (nullptr == event)
    {
        return;
    }
    if (Qt::LeftButton != event->buttons() || nullptr == newArrow_)
    {
        return QGraphicsScene::mouseMoveEvent(event);
    }

    const QPointF &scenePos = event->scenePos();
    switch (curState_)
    {
        case State::DrawArrow:
        {
            newArrow_->setEndPointForce(scenePos);

            auto foundedItem = dynamic_cast<UniversalGraphicsItem*>(findConnectionableItem(scenePos));
            if (nullptr != foundedItem && foundedItem != sourceConnectionableItem_)
            {
                foundedItem->setPortBarColor(foundedUGIPortBarColor_);
            }

            auto lastUniversalItem = dynamic_cast<UniversalGraphicsItem*>(lastFoundedConnectionableItem_);
            if (nullptr != lastUniversalItem && lastUniversalItem != sourceConnectionableItem_ && lastUniversalItem != foundedItem)
            {
                lastUniversalItem->setPortBarColorToDefault();
            }

            lastFoundedConnectionableItem_ = foundedItem;
            break;
        }
        default:
        {
            break;
        }
    }

    QGraphicsScene::mouseMoveEvent(event);
}

void FlowScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (nullptr == event)
    {
        return returnMouseReleaseEvent(event);
    }
    if (Qt::LeftButton != event->button())
    {
        return returnMouseReleaseEvent(event);
    }
    switch (curState_)
    {
        case State::DrawArrow:
        {
            if (nullptr == newArrow_)
            {
                break;
            }
            if (nullptr == sourceConnectionableItem_)
            {
                cleanUpAfterWrongArrowDrawing();
                break;
            }
            auto* destinationConnectionableItem = findConnectionableItem(event->scenePos());
            if (nullptr == destinationConnectionableItem || destinationConnectionableItem == sourceConnectionableItem_)
            {
                cleanUpAfterWrongArrowDrawing();
                break;
            }
            addArrowController(sourceConnectionableItem_, destinationConnectionableItem, newArrow_);
            setState(State::AfterDrawArrow);
            break;
        }
        default:
        {
            break;
        }
    }
    returnMouseReleaseEvent(event);
}

void FlowScene::checkSelectionsBeforePressEvent(QGraphicsSceneMouseEvent* event)
{
    for (auto item : items(event->scenePos()))
    {
        pressedItem_ = dynamic_cast<ResizableItem*>(item);
        if (pressedItem_)
        {
            break;
        }
    }

    auto isCtrl = event->modifiers().testFlag(Qt::ControlModifier);

    if (pressedItem_ && pressedItem_->isSelected() && !isCtrl)
    {
        isNeedToSpecialSelection_ = true;
    }
}

void FlowScene::checkSelectionsAfterReleaseEvent()
{
    if (!isNeedToSpecialSelection_)
    {
        return;
    }

    clearSelection();
    isNeedToSpecialSelection_ = false;
    prevSelectedItems_.clear();
    auto savedSelectionOrder = selectedAlignableItems_;
    selectedAlignableItems_.clear();

    for (auto selOrderItem : savedSelectionOrder)
    {
        if (selOrderItem != pressedItem_ && selOrderItem)
        {
            selOrderItem->setSelected(true);
            emit selectionChanged();
        }
    }
    pressedItem_->setSelected(true);
    emit selectionChanged();
}

void FlowScene::selectionHasChanged()
{
    if (!isNeedToSpecialSelection_)
    {
        handleStandardSelection(false);
        handleStandardSelection(true);

        prevSelectedItems_ = selectedResizableItems();
    }

    bool canResize = selectedResizableItems().count() <= 1;
    for (auto alignItem : selectedAlignableItems_)
    {
        if (alignItem)
        {
            alignItem->enableResizing(canResize);
        }
    }
}

void FlowScene::handleStandardSelection(bool isSelectMode)
{
    auto list = detectNewItems(isSelectMode);
    for (auto item : list)
    {
        if (item)
        {
            markLastOrderItem(false);
            if (isSelectMode)
            {
                selectedAlignableItems_.append(item);
            }
            else
            {
                selectedAlignableItems_.removeOne(item);
            }
            markLastOrderItem(true);
        }
    }
}

QList<ResizableItem*> FlowScene::detectNewItems(bool isSelectMode) const
{
    QList<ResizableItem*> retList;

    auto leftList = isSelectMode ? prevSelectedItems_ : selectedResizableItems();
    auto rightList = isSelectMode ? selectedResizableItems() : prevSelectedItems_;

    for (auto rightItem : rightList)
    {
        if (!leftList.contains(rightItem))
        {
            retList.append(rightItem);
        }
    }
    return retList;
}

void FlowScene::markLastOrderItem(bool isMarked)
{
    if (selectedAlignableItems_.size() >= 2)
    {
        auto resItem = selectedAlignableItems_.last();
        if (resItem)
        {
            resItem->setMainForAlignment(isMarked);
        }
    }
}

QList<ResizableItem*> FlowScene::selectedResizableItems() const
{
    QList<ResizableItem*> resItems;
    for (auto item : selectedItems())
    {
        auto resItem = dynamic_cast<ResizableItem*>(item);
        if (resItem)
        {
            resItems.append(resItem);
        }
    }
    return resItems;
}

void FlowScene::updateSceneRect()
{
    auto itemRect = itemsBoundingRect();
    if (!TinyMath::isRectValid(itemRect))
    {
        return;
    }
    for (const auto &view : views())
    {
        if (dynamic_cast<MiniView*>(view))
        {
            return;
        }
        auto viewRect = view->mapToScene(view->rect()).boundingRect();
        auto unitedRect = viewRect.united(itemRect).united(sceneRect());
        setSceneRect(unitedRect);
    }
}

void FlowScene::setBackColor(const QColor &color)
{
    backColor_ = color;
}

void FlowScene::setConnectionsVisible(const QSet<const Connection*> &connections, bool newVisible)
{
    auto arrowControllers = arrowControllers_;
    for (auto arrowController : arrowControllers)
    {
        auto connectionArrow = dynamic_cast<Arrows::ConnectionArrow*>(arrowController->arrow());

        if (nullptr != connectionArrow)
        {
            connectionArrow->setConnectionsVisible(connections, newVisible);
        }
    }
}

void FlowScene::setConnectionsProperty(const QSet<const QtNodes::Connection*> &connections,
                                       Item::GraphicsItemProperties property, QVariant value)
{
    auto arrowControllers = arrowControllers_;
    for (auto arrowController : arrowControllers)
    {
        auto connectionArrow = dynamic_cast<Arrows::ConnectionArrow*>(arrowController->arrow());
        if (nullptr != connectionArrow)
        {
            bool intersects = connections.intersects(connectionArrow->connections());
            if (intersects)
            {
                connectionArrow->setProperty(property, value);
            }
        }
    }
}

void FlowScene::hideConnections(const QSet<const Connection*> &hidingConnections)
{
    setConnectionsVisible(hidingConnections, false);
}

void FlowScene::showConnections(const QSet<const Connection*> &showingConnections)
{
    setConnectionsVisible(showingConnections, true);
}

std::set<ArrowController*> FlowScene::arrowControllers() const
{
    qDebug() << "FlowScene:" << this->id() << "getArrowControllers";
    return arrowControllers_;
}

void FlowScene::keyPressEvent(QKeyEvent* event)
{
    int keyCode = event->key();
    if (keyCode == Qt::Key_Up || keyCode == Qt::Key_Down || keyCode == Qt::Key_Left || keyCode == Qt::Key_Right)
    {
        return;
    }

    switch (event->key())
    {
        case Qt::Key_Delete:
        {
            deleteSelectedNodes();
            break;
        }
        default:
        {
            break;
        }
    }

    QGraphicsScene::keyReleaseEvent(event);
}

void FlowScene::helpEvent(QGraphicsSceneHelpEvent* helpEvent)
{
    QGraphicsScene::helpEvent(helpEvent);
}

void FlowScene::completeUpdateItemId(UniversalSelectableItem* item)
{
    auto found = std::find_if(graphicsItems_.begin(), graphicsItems_.end(), [item](auto pair)
    {
        if (pair.second == item)
        {
            return true;
        }
        return false;
    });

    if (found != graphicsItems_.end())
    {
        auto entry = graphicsItems_.extract(found);
        entry.key() = item->id();
        graphicsItems_.insert(std::move(entry));
    }
}

QPointF FlowScene::barPos() const
{
    return barPos_;
}
