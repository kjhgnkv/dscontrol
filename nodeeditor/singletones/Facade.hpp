#pragma once

#include "models/DataFlowModel.hpp"
#include "nodes/Serializable.hpp"

#include <memory>
#include "common/nodeeditor_globals.hpp"

class QMimeData;
// class DataImpRegistry;

namespace QtNodes
{
class DataFlowModel;
class NodesScene;
class MessageList;
class CommunicationList;
class NodeIndex;
class FlowScene;
class UniversalGraphicsItem;
class UniversalGraphicsItemImpl;
class UniversalSelectableItem;
class FrameGraphicsItem;
struct CutData;

class Facade : public QObject, public Serializable
{
Q_OBJECT
public:
    friend DataFlowModel;
    static Facade &Instance();

    Facade(Facade const &) = delete;
    Facade &operator=(Facade const &) = delete;

    Facade(Facade &&) = delete;
    Facade &operator=(Facade &&) = delete;

    NodeIndex createNode(const Item::NodeType &type, const Item::DeploymentType &deployType, const QJsonObject &info, const QString &filePath);
    void createComposite(const NodeIndex &index, const QPointF &pos, const QUuid &parentScene);
    void updateItemTitleVisibility(UniversalGraphicsItem* item, UniversalGraphicsItemImpl* impl);

    void renameNode(const NodeIndex &index);

    void connectComposite(UniversalGraphicsItem* item);
    void connectBoundaryFrame(FrameGraphicsItem* item);

    bool createConnection(const NodeIndex &lNode, const NodeIndex &rNode, 
        const NodeIndex &sNode, CommId commId, MsgId msgId);

    bool removeConnection(const NodeIndex &index, const QUuid &uuid);
    bool removeConnections(const NodeIndex &index);
    bool removeConnections(const QSet<const Connection*> connections);

    std::shared_ptr<FlowScene> scenePart(const QUuid &uid, qreal scale);
    std::shared_ptr<FlowScene> sceneMain();

    DataFlowModel* model() const;
    NodesScene* sceneModel() const;

    int addCommunication();
    bool removeCommunication(const int &id);
    CommunicationList* communicationList();
    MessageList* messageList();

    QString generateComponentOwnership(const NodeIndex &index);

    void connectTwoItems(const NodeIndex &idxOut, const NodeIndex &idxIn, int commId, const QUuid &connectionId,
                         int outSide, int inSide);
    void disconnectTwoItems(const NodeIndex &idxOut, const NodeIndex &idxIn, const QUuid &connectionId);

    void clear();

    void extractCode(const QString &name);

public slots:
    void removeComposite(const QUuid &id);
    void createGraphicsConnect(ConnectionID id);
    QMimeData* copy();
    std::list<CutData> cut(QList<QGraphicsItem*> item);
    std::list<CutData> cut();
    void paste(QMimeData* data, const QUuid &sceneId, QPointF pos = {20, 20});
    void paste(std::list<CutData> data, const QUuid &sceneId, QPointF pos);

    void nodeDropped(const QPointF &pos, const QUuid &source, const QUuid &parent, Item::NodeType type,
                     Item::DeploymentType deployType, const QJsonObject &info, const QString &path);

private slots:
    void removeNodeGraphics(const NodeIndex &index);
    bool canCopy(QList<QGraphicsItem*> items);
    bool canCut(QList<QGraphicsItem*> items);
    bool canPaste(QMimeData* data, const QUuid &sceneId);
    bool canPaste(std::list<CutData> data, const QUuid &sceneId);

signals:
    void itemDoubleClicked(const QUuid &itemId);
    void itemInfoChanged(QRectF, QString);
    void itemPropertiesChanged();
    void itemSelected(bool selected, const QUuid &id);
    void projectChanged();
    void itemRemoved();
    void nodeRenamed(const NodeIndex &);
    void extractComponentCode(const QString &name);
    void runDesigner(const QString &filename);

private:
    std::pair<int, int> getPortsSides(const NodeIndex &idxOut, const NodeIndex &idxIn);
    bool needWaitForOtherSide(const NodeIndex &idxMe, const NodeIndex &idxOther);

    std::map<QUuid, ConnectionID> removeNodeConnections(const NodeIndex &index);
    void restoreNodeConnections(std::map<QUuid, ConnectionID>);

    NodesCountMap createNodesCountMap(const QJsonArray &nodes) const;
    bool canPasteByCount(const QJsonArray &nodes) const;
    bool canPasteHelper(Item::NodeType targetType, Item::NodeType type, Item::DeploymentType deploymentType) const;
    void showNodesMaxCountMessage(Item::NodeType type) const;

protected:
    Facade();
    ~Facade() override = default;

    DataFlowModel* nodesModel_;
    NodesScene* sceneModel_;
    MessageList* messageList_;
    CommunicationList* communicationList_;

    // DataImpRegistry *registry_;

    // Serializable interface
public:
    QJsonObject save() const override;
    QJsonObject unsafeSave() const; // TODO dont touch now! use experimental in deploy
    void restore(const QJsonObject &p) override;
};
} // namespace QtNodes
