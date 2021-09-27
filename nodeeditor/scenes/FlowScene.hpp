#pragma once

//
#include "nodeeditor/nodeeditor_export.h"
#include "common/SerializableIdentityUnit.h"
#include "common/nodeeditor_globals.hpp"
#include "nodes/QUuidStdHash.hpp"

//
#include <QGraphicsScene>
#include <QSet>
#include <QUuid>
#include <set>

class QMenu;
class QMimeData;

namespace QtNodes
{
class Node;
class NodeIndex;
class DataFlowModel;
class TextGraphicsItem;
class Connection;
class UniversalGraphicsItem;
class FrameGraphicsItem;
class UniversalConnectionableItem;
class UniversalSelectableItem;
class ResizableItem;
class ArrowController;
class AddGraphicsFrameCommand;
class AddGraphicsTextCommand;
class AddGraphicsArrowCommand;
class RemoveGraphicsArrowCommand;
class RemoveGraphicsFrameCommand;
class RemoveGraphicsTextCommand;
class Facade;
class FlowView;
class NoteGraphicsItem;

namespace Arrows
{
class AbstractArrow;
class ConnectionArrow;
} // namespace Arrows

/// The FlowScene is responsible for rendering a FlowScene
/// If you're looking for a basic dataflow model, see DataFlowScene
class NODEEDITOR_EXPORT FlowScene : public QGraphicsScene, public SerializableIdentityUnit
{
    friend AddGraphicsFrameCommand;
    friend AddGraphicsTextCommand;
    friend AddGraphicsArrowCommand;
    friend RemoveGraphicsArrowCommand;
    friend RemoveGraphicsFrameCommand;
    friend RemoveGraphicsTextCommand;
    friend Facade;

Q_OBJECT

public:
    enum class State
    {
        Normal,
        AddFrame,
        AddArrow,
        DrawArrow,
        AfterDrawArrow
    };

    enum class DragMode
    {
        NoDrag,
        RubberBand
    };

    FlowScene(Item::NodeType type, const QUuid &id, Item::DeploymentType deploymentType, QObject* parent = nullptr);
    FlowScene(QObject* parent = nullptr);
    ~FlowScene() override;

    void changeArrowType(const QUuid &arrowId, const QtNodes::Arrows::ArrowType &arrowType);

    /**\return node graphics item by uid. If has no - nullptr
     */
    UniversalSelectableItem* graphicsItem(const QUuid &id) const;

    ArrowController* arrowController(const QUuid &arrowId) const;

    /**\brief not set ownership of model
     */
    void setFlowModel(DataFlowModel* model);

    /**
     * @brief get current nodes model
     * @return NodesModel
     */
    DataFlowModel* nodesModel() const;

    /**
     * @brief draws graphics item of node with target uid on the scene and adds
     * this item to the all-items container
     * @param item
     * @param uid
     */
    void addUniversalGraphicsItem(UniversalGraphicsItem* item, const QPointF &pos);
    bool addGraphicsItem(UniversalSelectableItem* item, const QPointF &pos);

    UniversalSelectableItem* addGraphicsFrame(QPointF pos, QSizeF size = {250, 250});

    std::vector<ResizableItem*> getResizableItems() const;

    FlowView* flowView() const;

    /**
     * @brief adds Notes item connected to target item
     */
    QtNodes::NoteGraphicsItem* addNoteItem(QPointF pos, QSizeF size, const QString &text, UniversalGraphicsItem* item);

    /**
     * @brief associate item with target parent item
     */
    void associateItemWith(const QUuid &id, const QUuid &parentId);

    /**
     * @return all associated items
     */
    std::set<QUuid> associatedItems(const QUuid &id);

    /**
     * @brief adds TextBox-based note item connected to target item
     */
    TextGraphicsItem* addTextBoxNote(QPointF pos, QSizeF size, UniversalGraphicsItem* item);

    UniversalSelectableItem* addTextItem(const QPointF &pos, const QUuid &parentId = {});

    ArrowController* addArrowController(UniversalConnectionableItem* source, UniversalConnectionableItem* dest,
                                        Arrows::AbstractArrow* arrow);

    /**
     * @brief returns all items which bounding rectangle fully contained in
     * target rectangle
     */
    QSet<QGraphicsItem*> itemsInRect(const QRectF &rect) const;

    QSet<QGraphicsItem*> itemsInPath(const QPainterPath &path) const;

    QSet<UniversalConnectionableItem*> connectionableItemsInPath(const QPainterPath &path) const;

    QSet<Arrows::AbstractArrow*> arrowsInRect(const QRectF &rect) const;

    /**
     * @brief removes graphics item of node with target uid from the scene and
     * from all-items container
     */
    UniversalSelectableItem* removeGraphicsItem(const QUuid &uid);
    void removeGraphicsItem(UniversalSelectableItem* item);

    void deleteGraphicsItem(UniversalSelectableItem* item);
    void deleteGraphicsItem(const QUuid &uid);

    /**
     * @brief returns all-items container
     */
    std::map<QUuid, UniversalSelectableItem*> graphicsItems() const;

    /**
     * @return all selected items which can be aligned
     */
    QList<ResizableItem*> selectedAlignableItems() const;
    /**
     * @return true if item is one of selected alignable items
     */
    bool isOneOfSelectedAlignable(UniversalGraphicsItem* item);
    /**
     * @return first found item that can be aligned at pos
     */
    UniversalGraphicsItem* graphicsItemAt(const QPointF &pos) const;

    /**
     * @brief draws connection line between two node graphics items ports
     */

    void createLineConnection(const QUuid &outItem, const QUuid &outPort, const QUuid &inItem, const QUuid &inPort,
                              const QUuid &communicationId);

    /**
     * @brief draws connection line between node graphics items port and graphic
     * item that is the representation of parent item port on the scene
     */
    void createLineConnection(const QUuid &outItem, const QUuid &outPort, UniversalConnectionableItem* internalPort,
                              const QUuid &communicationId);

    void removeLineConnection(const QUuid &itemIdx, const QUuid &communicationId);

    void addPort(const QUuid &id);

    void setDragMode(const DragMode &mode);
    DragMode dragMode() const;

    void paste(const QJsonObject &node);

    /**
     * @brief create context menu of scene
     * \warning this menu dynamically allocated, so its have to be deleted after
     * use
     */
    QMenu* createContextMenu(QPointF pos);

    QJsonObject save() const override;
    void restore(const QJsonObject &obj) override;

    void restoreConnections(const QJsonObject &p);

    std::set<ArrowController*> arrowControllers() const;

    void setBackColor(const QColor &color);

    void setConnectionsVisible(const QSet<const QtNodes::Connection*> &connections, bool newVisible);

    void hideConnections(const QSet<const QtNodes::Connection*> &hidingConnections);

    void setConnectionsProperty(const QSet<const QtNodes::Connection*> &connections,
                                Item::GraphicsItemProperties property, QVariant value);

    void showConnections(const QSet<const QtNodes::Connection*> &showingConnections);

    QPointF barPos() const;

    /**
     * @brief set current state. Note, when you set AddFrame as current state,
     * then after press on scene new Frame will be added, and scene returns back
     * to normal state
     */
    void setState(const State &state);
    /**\return current state. By default - Normal
     */
    State state() const;

    void setNewArrowType(const QtNodes::Arrows::ArrowType &type);
    QtNodes::Arrows::ArrowType newArrowType() const;

    Item::NodeType type() const;
    Item::DeploymentType deploymentType() const;

    void alignByGrid();

private:
    /**
     * @brief draws graphics frame on the scene and adds
     * this frame to the all-frames container
     */
    UniversalSelectableItem* addGraphicsFrame(QPointF pos, QSizeF size, const QJsonObject &restore);
    TextGraphicsItem* addTextItem(const QPointF &pos, const QJsonObject &restore, const QUuid &parentId = {});
    ArrowController* addArrowController(UniversalConnectionableItem* source, UniversalConnectionableItem* dest,
                                        Arrows::AbstractArrow* arrow, const QJsonObject &restore);

    void deleteGraphicsItemCommand(UniversalSelectableItem* item);
    void deleteGraphicsItemCommand(const QUuid &uid);

    void moveOnPositions(const QList<QList<ResizableItem*>> &alignItems, qint32 rowCount, qint32 columnCount);
    qreal detectMaximumItemSize(const QList<ResizableItem*> &alignItems, bool isWidth) const;

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* conn) override;

    void dragEnterEvent(QGraphicsSceneDragDropEvent* event) override;
    void dragMoveEvent(QGraphicsSceneDragDropEvent* event) override;
    void dragLeaveEvent(QGraphicsSceneDragDropEvent* event) override;
    void dropEvent(QGraphicsSceneDragDropEvent* event) override;

    /**\brief this method check could be mime data drop on the scene (not on
     * item) or not \return true if mime data can be droped, not if can not be
     * \param pos scenePos
     */
    virtual bool canDropMimeData(const QPointF pos, const QMimeData* data);

    /**\brief list of acceptable mime formats
     */
    virtual QStringList mimeTypes() const;

    void mousePressEvent(QGraphicsSceneMouseEvent* mousePressEvent) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void helpEvent(QGraphicsSceneHelpEvent* helpEvent) override;
    // void keyReleaseEvent(QKeyEvent *event) override;

    void completeUpdateItemId(UniversalSelectableItem* item);

public slots:
    void deleteSelectedNodes();

    void updateSceneRect();

    void arrowChangingType(const Arrows::ArrowType &newArrowType);

signals:
    void fontChanged(const QFont &font, const QColor &color);

    /**\brief
     *  emits when change state happens
     */
    void stateChanged(const State &state);

    /**\brief emits when node dropped on scene
     */
    void nodeDropped(const QPointF &pos, const QUuid &source, const QUuid &parent, Item::NodeType type,
                     Item::DeploymentType deployType, const QJsonObject &info, const QString &path);

    /**\brief emits when Selected node should be deleted (<Del button>)
     */
    void nodeDeleted(const QUuid &uid);

    /**\brief emits when scenes bounding rect changes
     */
    void sceneRectChanged();

    /**\brief emits on mouse press on scene and on unselect all items
     */
    void itemsUnselected();

protected slots:
    /**\brief clear model and scene
     */
    void clearAll(const bool &withChild = true);
    void destroyArrowController(); ///< Added by Nick

private:
    void createNewArrow(const QPointF &scenePos);
    void deleteNewArrow();
    void returnMouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    void cleanUpAfterWrongArrowDrawing();
    bool addConnectionToPort(const QUuid &portId, const QUuid &connId);

    void removeConnectionFromPort(const QUuid &portId, const QUuid &connId);

    void removePort(const QUuid &portId);

    UniversalConnectionableItem* findConnectionableItem(const QPointF &scenePos) const;
    FrameGraphicsItem* findBoudary(const QPointF &scenePos) const;
    UniversalGraphicsItem* findUniversalGraphicsItem(const QPointF &scenePos) const;
    Arrows::AbstractArrow* createArrow(Arrows::ArrowType type) const;

    void clear();

    // For special selection
    void checkSelectionsBeforePressEvent(QGraphicsSceneMouseEvent* event);
    void checkSelectionsAfterReleaseEvent();
    void selectionHasChanged();
    void handleStandardSelection(bool isSelectMode);
    /*!
     * \brief Detect new selected or unselected resizable items depending on the argument
     * \param isSelectMode - true, when need to detect new selected items; false - unselected
     * \return list of this resizable items
     */
    QList<ResizableItem*> detectNewItems(bool isSelectMode) const;
    void markLastOrderItem(bool isMarked);
    QList<ResizableItem*> selectedResizableItems() const;

private:
    DataFlowModel* model_;

    std::map<QUuid, UniversalSelectableItem*> graphicsItems_;
    std::multimap<QUuid, QUuid> connectionLines_;
    UniversalConnectionableItem* lastFoundedConnectionableItem_;

    std::map<QUuid, std::set<QUuid>> ports_;
    std::map<QUuid, std::set<QUuid>> associations_;

    // For special selection
    QList<ResizableItem*> prevSelectedItems_;
    QList<ResizableItem*> selectedAlignableItems_;
    bool isNeedToSpecialSelection_;
    ResizableItem* pressedItem_;

    std::set<ArrowController*> arrowControllers_; ///< Added by Nick

    QPointF lastPos_;

    QtNodes::Arrows::ArrowType newArrowType_; ///< Added by Nick
    Arrows::AbstractArrow* newArrow_;
    UniversalConnectionableItem* sourceConnectionableItem_;

    State curState_;

    Item::NodeType type_;
    QtNodes::Item::DeploymentType deploymentType_;
    QColor backColor_;
    const QColor foundedUGIPortBarColor_;

    QPointF barPos_;

    DragMode dragMode_;

    QUuid lastUid_;

    bool isSingleMouseButton_;
};
} // namespace QtNodes
