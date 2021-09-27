// DataNodesModel.hpp

#pragma once

//
#include "nodeeditor/nodeeditor_export.h"

//
#include "lists/Message.hpp"
#include "NodesModel.hpp"
#include "nodes/ConnectionID.hpp"
#include "nodes/NodeIndex.hpp"
#include "nodes/QUuidStdHash.hpp"
#include "nodes/Serializable.hpp"
#include "common/nodeeditor_globals.hpp"

//
#include <functional>
#include <map>
#include <memory>

namespace QtNodes
{

using NodesCountMap = std::map<Item::NodeType, size_t>;

class NodeImp;
class Connection;
class Node;
class AddNodeCommand;
class RemoveNodeCommand;
class RemoveConnectionCommand;
class AddConnectionCommand;

/**
 * @brief Main flow data model.
 * @note This model store values in order from less to greater.
 */
class NODEEDITOR_EXPORT DataFlowModel : public NodesModel, public Serializable
{
Q_OBJECT
    friend NameController;
    friend AddNodeCommand;
    friend AddConnectionCommand;
    friend RemoveNodeCommand;
    friend RemoveConnectionCommand;

public:
    explicit DataFlowModel(QObject* parent = nullptr);

    size_t size() const override;
    bool empty() const override;

    NodeIterator begin() const override;
    NodeIterator end() const override;

    Item::NodeType nodeType(const NodeIndex &index) const override;
    Item::DeploymentType nodeDeploymentType(const NodeIndex &index) const override;
    NodeIndex nodeIndex(const QUuid &id) const override;
    std::pair<bool, std::map<QUuid, std::shared_ptr<Node>>::iterator> removeNode(const NodeIndex &index) override;
    bool removeNodes(const QList<Node*> index) override;
    bool addConnection(NodeIndex const &leftNode, NodeIndex const &rightNode, NodeIndex const &serverNodeIdx, CommId commId, MsgId msgId) override;

    // ------------------------------------------------------------------------
    // interface to talk with NodeIndex

    Node* nodePointer(const NodeIndex &index) const override;
    QString nodeCaption(NodeIndex const &index) const override;
    QWidget* nodeWidget(NodeIndex const &index) const override;
    std::list<std::shared_ptr<Connection>> nodeConnections(const NodeIndex &index) const override;
    std::map<NodeIndex, std::list<std::shared_ptr<Connection>>> nodeConnectionsInDepth(const NodeIndex &index) const;

    NodeIndex nodeParent(const NodeIndex &index) const override;
    QList<NodeIndex> nodeChilds(const NodeIndex &index) const override;
    QList<NodeIndex> nodeChildsInLowestLevel(const NodeIndex &index) const override;

    void nodeRestore(const NodeIndex &index, const QJsonObject &object) const override;
    QJsonObject nodeSave(const NodeIndex &index) const override;

    Connection* connectionPointer(const ConnectionID &id) const override;

    void nodeSetInfo(const NodeIndex &index, const QJsonObject &info, const QString &filePath);

    //-------------------------------------------------------------------------

    QJsonObject save() const override;
    void restore(const QJsonObject &data) override;
    void restoreConnection(const QJsonObject &data);

    void paste(const QJsonObject &data);
    void setRelation(const NodeIndex &index, const NodeIndex &parent) override;
    QList<NodeIndex> nodes(Item::NodeType type) override;
    const std::map<QUuid, std::shared_ptr<Node>> &nodes() const;
    std::list<QtNodes::Message> nodeMessages(const NodeIndex &index, Item::PortType type) const;
    NodeImp* nodeImplementation(const NodeIndex &index) const;

    void clear() override;
    QJsonObject unsafeSave() const;

    void nodeChangeCaption(const NodeIndex &index, const QString &caption) const;

    QJsonObject nodeCopy(const NodeIndex &index) const override;
    void nodePaste(const QJsonObject &node) override;

    const std::unordered_map<ConnectionID, std::shared_ptr<Connection>> &connections() const;
    void restoreConnections(const QJsonObject &p);

    bool canAddNode(Item::NodeType type) const;
    bool canAddNodes(Item::NodeType type, size_t count) const;
    std::pair<bool, Item::NodeType> canAddNodes(const NodesCountMap &nodesCountMap) const;

public slots:
    bool removeConnection(const ConnectionID &id) override;
    NodeIndex addNode(Item::NodeType type, Item::DeploymentType deployType, const QJsonObject &restore = {}) override;

protected:
    // convenience functions
    NodeIndex addNode(QUuid const &uuid, Item::NodeType type, Item::DeploymentType deployType,
                      const QJsonObject &restore = {});

    NodeIndex addNode(NodeImp* model, QUuid const &uuid, Item::NodeType type, Item::DeploymentType deployType,
                      const QJsonObject &restore = {});

    ConnectionID addConnection(Node* left, Node* right, Node* server, CommId commId, MsgId msgId);
    ConnectionID addConnection(Node* leftNode, Node* rightNode, Node* server, const QJsonObject &restore);
    ConnectionID addConnection(Node* leftNode, Node* rightNode, std::shared_ptr<Connection> conn);
    std::pair<bool, std::map<QUuid, std::shared_ptr<Node>>::iterator> removeNodeCommand(
    const NodeIndex &index) override;
    bool removeConnectionCommand(const ConnectionID &id);

signals:
    void nodeRestored(const NodeIndex &index);

private:
    static NodesCountMap createDefaultMaxNodesCountMap();
    void nodeSetCaption(const NodeIndex &index, const QString &caption) const override;
    size_t nodesCount(Item::NodeType type) const;
    size_t maxNodesCount(Item::NodeType type) const;

protected:
    std::map<QUuid, std::shared_ptr<Node>> nodes_;
    std::unordered_map<ConnectionID, std::shared_ptr<Connection>> connections_;
    QUuid lastUid_;

private:
    const NodesCountMap maxNodesCount_;
};
} // namespace QtNodes
