#include "DataFlowModel.hpp"

#include "nodes/ApplicationNode.hpp"
#include "nodes/ComponentNode.hpp"
#include "nodes/ComputerNode.hpp"
#include "nodes/Node.hpp"
#include "nodes/SubSystemNode.hpp"
#include "nodes/SystemNode.hpp"
#include "nodes/Connection.hpp"
#include "lists/MessageList.hpp"
#include "DataFlowModelIteratorImpl.hpp"
#include "nodes/ApplicationItemImpl.h"
#include "nodes/ComponentItemImpl.h"
#include "nodes/ComputerItemImpl.h"
#include "nodes/SubSystemItemImpl.h"
#include "nodes/SystemItemImpl.h"
#include "factories/CommandFactory.hpp"
#include "lists/CommunicationList.hpp"
#include "singletones/Facade.hpp"
#include "singletones/NameController.hpp"
#include "singletones/UndoController.hpp"
#include "utils/undo/AddConnectionCommand.hpp"
#include "utils/undo/RemoveNodeCommand.hpp"
#include "utils/undo/AddNodeCommand.hpp"

#include <QApplication>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>

constexpr size_t maxMainScenesCount {1};
constexpr size_t maxSystemNodesCount {20};
constexpr size_t maxSubSystemNodesCount {40};
constexpr size_t maxComputerNodesCount {40};
constexpr size_t maxApplicationNodesCount {60};
constexpr size_t maxComponentNodesCount {100};
constexpr size_t maxOtherNodesCount {std::numeric_limits<size_t>::max()};

using namespace QtNodes;

DataFlowModel::DataFlowModel(QObject* parent)
: NodesModel {parent}
, maxNodesCount_ {createDefaultMaxNodesCountMap()}
{
    // when node removed, remove node name
    connect(this, &DataFlowModel::nodeAboutToBeRemoved, NameController::Instance(),
            &NameController::itemCaptionRemovedSlot);
}

NodesCountMap DataFlowModel::createDefaultMaxNodesCountMap()
{
    NodesCountMap ret;

    for (auto i {static_cast<int>(Item::NodeType::None)}; i <= static_cast<int>(Item::NodeType::OtherType); ++i)
    {
        Item::NodeType nodeType = static_cast<Item::NodeType>(i);
        switch (nodeType)
        {
            case Item::NodeType::MainScene:
            {
                ret.insert_or_assign(nodeType, maxMainScenesCount);
                break;
            }
            case Item::NodeType::SystemNode:
            {
                ret.insert_or_assign(nodeType, maxSystemNodesCount);
                break;
            }
            case Item::NodeType::SubSystemNode:
            {
                ret.insert_or_assign(nodeType, maxSubSystemNodesCount);
                break;
            }
            case Item::NodeType::ComputerNode:
            {
                ret.insert_or_assign(nodeType, maxComputerNodesCount);
                break;
            }
            case Item::NodeType::ApplicationNode:
            {
                ret.insert_or_assign(nodeType, maxApplicationNodesCount);
                break;
            }
            case Item::NodeType::ComponentNode:
            {
                ret.insert_or_assign(nodeType, maxComponentNodesCount);
                break;
            }
            default:
            {
                ret.insert_or_assign(nodeType, maxOtherNodesCount);
                break;
            }
        }
    }
    return ret;
}

size_t DataFlowModel::size() const
{
    return nodes().size();
}

Item::NodeType DataFlowModel::nodeType(const NodeIndex &index) const
{
    auto found = nodes().find(index.id());
    if (found != nodes().end())
    {
        return found->second->type();
    }
    return {};
}

Item::DeploymentType DataFlowModel::nodeDeploymentType(const NodeIndex &index) const
{
    auto found = nodes().find(index.id());
    if (found != nodes().end())
    {
        return found->second->deploymentType();
    }
    return {};
}

NodeIndex DataFlowModel::nodeIndex(const QUuid &id) const
{
    auto iter = nodes().find(id);
    if (iter == nodes().end())
    {
        return createNotValidIndex(id);
    }

    return createIndex(id, iter->second.get());
}

Node* DataFlowModel::nodePointer(const NodeIndex &index) const
{
    auto found = nodes().find(index.id());
    if (found != nodes().end())
    {
        return found->second.get();
    }
    return {};
}

QString DataFlowModel::nodeCaption(NodeIndex const &index) const
{
    if (auto node = nodePointer(index))
    {
        return node->caption();
    }
    return {};
}

void DataFlowModel::nodeSetCaption(const NodeIndex &index, const QString &caption) const
{
    if (index.isValid())
    {
        if (auto impl = nodeImplementation(index); impl)
        {
            impl->setCaption(caption);
        }
    }
}

size_t DataFlowModel::nodesCount(Item::NodeType type) const
{
    size_t nodesCount {0};
    std::shared_ptr<Node> tmpNode {nullptr};
    for (const auto &node : nodes_)
    {
        tmpNode = node.second;
        if (nullptr != tmpNode && tmpNode->type() == type)
        {
            nodesCount++;
        }
    }
    return nodesCount;
}

size_t DataFlowModel::maxNodesCount(Item::NodeType type) const
{
    const size_t ret = maxNodesCount_.at(type);
    return ret;
}

bool DataFlowModel::canAddNode(Item::NodeType type) const
{
    const size_t nodesCount {1};
    return canAddNodes(type, nodesCount);
}

bool DataFlowModel::canAddNodes(Item::NodeType type, size_t count) const
{
    return nodesCount(type) + count <= maxNodesCount(type);
}

std::pair<bool, Item::NodeType> DataFlowModel::canAddNodes(const NodesCountMap &nodesCountMap) const
{
    for (const auto &[type, count] : nodesCountMap)
    {
        if (!canAddNodes(type, count))
        {
            return {false, type};
        }
    }
    return {true, Item::NodeType::None};
}

QJsonObject DataFlowModel::nodeCopy(const NodeIndex &index) const
{
    if (auto node = index.nodePointer())
    {
        return node->unsafeSave();
    }
    return {};
}

void DataFlowModel::nodePaste(const QJsonObject &node)
{
    qDebug() << "DataNodesModel: restore";

    auto restore = node;

    auto name = node["name"].toString();
    auto type = static_cast<Item::NodeType>(node["type"].toInt());
    auto depl = (Item::DeploymentType)node["implementation"].toObject()["deploymentType"].toInt();
    auto id = QUuid::createUuid().toString();
    restore["id"] = id;

    // TODO: add deployment type to save\restore
    addNode(type, depl, restore);

    auto childs = node["childs"].toArray();

    for (const auto &child : childs)
    {
        auto tmp = child.toObject();
        auto impl = tmp["implementation"].toObject();
        impl["parentScene"] = id;
        tmp["implementation"] = impl;
        tmp["parent"] = id;
        nodePaste(tmp);
    }
}

void DataFlowModel::nodeChangeCaption(const NodeIndex &index, const QString &caption) const
{
    if (index.isValid())
    {
        NameController::Instance()->changeItemCaption(index, caption);
    }
}

QWidget* DataFlowModel::nodeWidget(NodeIndex const &index) const
{
    if (auto node = nodePointer(index))
    {
        return node->widget();
    }
    return nullptr;
}

std::list<std::shared_ptr<Connection>> DataFlowModel::nodeConnections(const NodeIndex &index) const
{
    if (auto type = nodeType(index); type == Item::NodeType::ComponentNode || type == Item::NodeType::ExternalNode)
    {
        if (auto node = nodePointer(index))
        {
            return node->connections();
        }
    }
    else
    {
        std::list<std::shared_ptr<Connection>> retval;
        auto childs = nodeChilds(index);
        for (const auto &child : childs)
        {
            auto conns = nodeConnections(child);
            retval.splice(retval.end(), conns);
        }
        return retval;
    }

    return {};
}

std::map<NodeIndex, std::list<std::shared_ptr<Connection>>> DataFlowModel::nodeConnectionsInDepth(
const NodeIndex &index) const
{
    std::map<NodeIndex, std::list<std::shared_ptr<Connection>>> retval {};

    if (auto type = nodeType(index); type == Item::NodeType::ComponentNode || type == Item::NodeType::ExternalNode)
    {
        if (auto node = nodePointer(index))
        {
            retval.insert({index, node->connections()});
            return retval;
        }
    }
    else
    {
        auto childs = nodeChilds(index);
        for (const auto &child : childs)
        {
            auto conns = nodeConnectionsInDepth(child);
            retval.insert(conns.begin(), conns.end());
        }
        return retval;
    }

    return {};
}

NodeIndex DataFlowModel::nodeParent(const NodeIndex &index) const
{
    if (auto node = nodePointer(index))
    {
        if (auto par = node->parentNode(); par)
        {
            auto idx = nodeIndex(par->id());
            Q_ASSERT(checkIndex(idx));
            return idx;
        }
    }

    return {};
}

QList<NodeIndex> DataFlowModel::nodeChilds(const NodeIndex &index) const
{
    QList<NodeIndex> ret;

    if (auto node = nodePointer(index))
    {
        for (const auto &i : node->childNodes())
        {
            ret.append(nodeIndex(i->id()));
        }
    }

    return ret;
}

QList<NodeIndex> DataFlowModel::nodeChildsInLowestLevel(const NodeIndex &index) const
{
    QList<NodeIndex> ret;

    if (auto node = nodePointer(index))
    {
        for (const auto &i : node->childNodesInLowestLevel())
        {
            ret.append(nodeIndex(i->id()));
        }
    }

    return ret;
}

bool DataFlowModel::removeConnection(const ConnectionID &id)
{
    auto command = UndoController::Instance().factory()->createRemoveConnectionCommand(this, id);
    if (command && command->isValid())
    {
        UndoController::Instance().push(command);
        return command->result();
    }

    delete command;
    return false;
}

bool DataFlowModel::addConnection(NodeIndex const &leftNodeIdx, NodeIndex const &rightNodeIdx,
    NodeIndex const &serverNodeIdx, CommId commId, MsgId msgId)
{
    Q_ASSERT(checkIndex(leftNodeIdx));
    Q_ASSERT(checkIndex(rightNodeIdx));

    auto command = UndoController::Instance().factory()->createAddConnectionCommand(this, leftNodeIdx, rightNodeIdx,
                                                                                    serverNodeIdx, commId, msgId);

    if (command)
    {
        UndoController::Instance().push(command);
        return command->result();
    }
    return false;
}

std::pair<bool, std::map<QUuid, std::shared_ptr<Node>>::iterator> DataFlowModel::removeNode(const NodeIndex &index)
{
    UndoController::Instance().begin("DataNodesModel: removeNode");

    auto command = UndoController::Instance().factory()->createRemoveNodeCommand(this, index, {});
    UndoController::Instance().push(command);

    UndoController::Instance().end();

    return reinterpret_cast<RemoveNodeCommand*>(command)->value();
}

std::pair<bool, std::map<QUuid, std::shared_ptr<Node>>::iterator> DataFlowModel::removeNodeCommand(
const NodeIndex &index)
{
    qDebug() << "DataNodesModel: remove node";

    // remove it from the map
    auto found = nodes_.find(index.id());
    if (found != nodes_.end())
    {
        for (const auto &i : nodeConnections(index))
        {
            removeConnection(i->id());
        }

        removeNodes(found->second->childNodes());

        emit nodeAboutToBeRemoved(index);
        // first we disconnect node with the model
        disconnect(index.nodePointer(), nullptr, this, nullptr);

        found->second->deleteLater();
        found->second = nullptr;
        auto founded = nodes_.find(index.id());
        auto it = nodes_.erase(founded);

        lastUid_ = index.id();

        emit nodeRemoved();
        return {true, it};
    }
    return {false, {}};
}

bool DataFlowModel::removeConnectionCommand(const ConnectionID &id)
{
    qDebug() << "DataNodesModel: remove connection";
    auto copyId = id;

    // check idx-s
    auto leftNodeIdx = nodeIndex(id.lNodeID_);
    auto rightNodeIdx = nodeIndex(id.rNodeID_);
    auto commId = id.commId_;

    Q_ASSERT(checkIndex(leftNodeIdx));
    Q_ASSERT(checkIndex(rightNodeIdx));

    auto leftNode = nodePointer(leftNodeIdx);
    auto rightNode = nodePointer(rightNodeIdx);
    if (!leftNode || !rightNode)
    {
        qWarning() << "DataNodesModel: node not found";
        return false;
    }

    // remove it from the nodes
    auto leftConns = leftNode->connections(QtNodes::Item::PortType::Out, commId);
    auto iter = std::find_if(leftConns.begin(), leftConns.end(), [&](std::shared_ptr<Connection> conn)
    {
        return conn->id() == id;
    });

    if (iter != leftConns.end())
    {
        // emit before remove
        emit connectionRemoved(copyId);
        leftNode->removeConnection(QtNodes::Item::PortType::Out, *iter);
        connections_.erase(copyId);
    }
    auto rightConns = rightNode->connections(QtNodes::Item::PortType::In, commId);
    iter = std::find_if(rightConns.begin(), rightConns.end(), [&](std::shared_ptr<Connection> conn)
    {
        return conn->id() == id;
    });

    if (iter != rightConns.end())
    {
        emit connectionRemoved(copyId);
        rightNode->removeConnection(QtNodes::Item::PortType::In, *iter);
        connections_.erase(copyId);
    }

    return true;
}

bool DataFlowModel::removeNodes(const QList<Node*> index)
{
    bool ret {false};
    for (const auto &node : index)
    {
        if (node)
            ret = removeNode(nodeIndex(node->id())).first;
    }

    return ret;
}

NodeIndex DataFlowModel::addNode(Item::NodeType type, Item::DeploymentType deployType, const QJsonObject &restore)
{
    if (!canAddNode(type))
    {
        return {};
    }

    UndoController::Instance().begin("DataNodesModel: add node");

    // auto idx = addNode(typeID, nodeid, type, deployType);

    auto command = UndoController::Instance().factory()->createAddNodeCommand(this, type, deployType, nullptr);

    command->setItemState(restore);

    UndoController::Instance().push(command);

    UndoController::Instance().end();

    return reinterpret_cast<AddNodeCommand*>(command)->index();
}

NodeIndex DataFlowModel::addNode(QUuid const &nodeid, Item::NodeType type, Item::DeploymentType deployType,
                                 const QJsonObject &restore)
{
    return addNode({}, nodeid, type, deployType, restore);
}

NodeIndex DataFlowModel::addNode(NodeImp* imp, QUuid const &nodeid, Item::NodeType type,
                                 Item::DeploymentType deployType, const QJsonObject &restore)
{

    if (!canAddNode(type))
    {
        return {};
    }

    std::shared_ptr<Node> node {nullptr};
    switch (type)
    {
        case Item::NodeType::ExternalNode:
        { // TODO: Make implementation for external
            imp = new ComponentItemImpl {{}, deployType};
            node = std::make_shared<ComponentNode>(imp, nodeid, type);
            break;
        }
        case Item::NodeType::ComponentNode:
        {
            imp = new ComponentItemImpl {{}, deployType};
            node = std::make_shared<ComponentNode>(imp, nodeid, type);
            break;
        }
        case Item::NodeType::ApplicationNode:
        {
            imp = new ApplicationItemImpl {deployType};
            node = std::make_shared<ApplicationNode>(imp, nodeid, type);
            break;
        }
        case Item::NodeType::ComputerNode:
        {
            imp = new ComputerItemImpl {deployType};
            node = std::make_shared<ComputerNode>(imp, nodeid, type);
            break;
        }
        case Item::NodeType::SubSystemNode:
        {
            imp = new SubSystemItemImpl {deployType};
            node = std::make_shared<SubSystemNode>(imp, nodeid, type);
            break;
        }
        case Item::NodeType::SystemNode:
        {
            // TODO: Why two deployment types?? remake to Item::DeploymentType?
            imp = new SystemItemImpl {deployType};
            node = std::make_shared<SystemNode>(imp, nodeid, type, Item::DeploymentType::Internal);
            break;
        }
        default:
        {
            return {};
        }
    }

    if (!restore.isEmpty())
    {
        node->restore(restore);
    }

    connect(node.get(), &Node::updated, this, [this, node]()
    {
        emit nodeUpdated(nodeIndex(node->id()));
    });

    // add it to the map
    if (!nodes_.insert(std::pair(node->id(), node)).second)
    {
        return {};
    }

    // tell the view
    auto index = nodeIndex(node->id());
    emit nodeAdded(index);
    lastUid_ = index.id();

    NameController::Instance()->itemCaptionAddedSlot(index, index.nodeCaption());
    return index;
}

ConnectionID DataFlowModel::addConnection(Node* leftNode, Node* rightNode, Node* serverNode, CommId commId, MsgId msgId)
{
    qDebug() << "DataNodesModel: add connection";
    if (!leftNode || !rightNode)
    {
        qWarning() << "DataNodesModel: non valid nodes";
        return {};
    }
    // create the connection

    auto list = Facade::Instance().communicationList();
    auto deviceType = list->communicationDeviceType(commId);
    auto deviceName = list->communicationDeviceName(commId);
    auto commName = list->communicationName(commId);

    auto conn = std::make_shared<Connection>(*rightNode, *leftNode, *serverNode, commId, deviceType, deviceName, commName, msgId);

    return addConnection(leftNode, rightNode, conn);
}

ConnectionID DataFlowModel::addConnection(Node* leftNode, Node* rightNode, Node* serverNode, const QJsonObject &restore)
{
    qDebug() << "DataNodesModel: add connection";
    if (!leftNode || !rightNode)
    {
        qDebug() << "DataNodesModel: non valid nodes";

        return {};
    }
    // create the connection
    auto list = Facade::Instance().communicationList();
    if (!list)
    {
        return {};
    }

    auto conn = std::make_shared<Connection>(*rightNode, *leftNode);
    conn->setServer(*serverNode);
    conn->restore(restore);
    return addConnection(leftNode, rightNode, conn);
}

ConnectionID DataFlowModel::addConnection(Node* leftNode, Node* rightNode, std::shared_ptr<Connection> conn)
{
    // check already existing connection
    if (const auto connections = leftNode->connections(QtNodes::Item::PortType::Out, conn->id().commId_);
    std::find_if(connections.begin(), connections.end(), [conn](const auto &val)
    {
        if (val->id() == conn->id())
        {
            return true;
        }
        return false;
    }) != connections.end())
    {
        return {};
    }

    {
        auto comms = Facade::Instance().messageList_->messageComms(
        Facade::Instance().messageList_->messageUid(conn->id().msgId_));
        auto found = std::find_if(comms.begin(), comms.end(), [conn](const auto &val)
        {
            if (val.id_ == conn->id().commId_)
            {
                return true;
            }
            return false;
        });
        if (found == comms.end())
        {
            return {};
        }
    }

    // check already existing connection
    if (const auto connections = rightNode->connections(QtNodes::Item::PortType::In, conn->id().commId_);
    std::find_if(connections.begin(), connections.end(), [conn](const auto &val)
    {
        if (val->id() == conn->id())
        {
            return true;
        }
        return false;
    }) != connections.end())
    {
        return {};
    }

    // add it to the nodes
    leftNode->addConnection(QtNodes::Item::PortType::Out, conn);
    rightNode->addConnection(QtNodes::Item::PortType::In, conn);

    auto[it, ret] = connections_.insert({conn->id(), conn});

    if (ret)
    {
        // tell the view the connection was added
        emit connectionAdded(conn->id());

        return conn->id();
    }

    Q_UNUSED(it);
    return {};
}

const std::map<QUuid, std::shared_ptr<Node>> &DataFlowModel::nodes() const
{
    return nodes_;
}

std::list<QtNodes::Message> DataFlowModel::nodeMessages(const NodeIndex &index, Item::PortType type) const
{
    if (auto found = nodes_.find(index.id()); found != nodes_.end())
    {
        // only component and external component have messages
        if (auto node = found->second.get(); node->type() == QtNodes::Item::NodeType::ComponentNode || node->type() == QtNodes::Item::NodeType::ExternalNode)
        {
            return node->messages(type);
        }
    }
    return {};
}

NodeImp* DataFlowModel::nodeImplementation(const NodeIndex &index) const
{
    if (auto found = nodes_.find(index.id()); found != nodes_.end())
    {
        if (auto node = found->second.get())
        {
            return node->imp();
        }
    }
    return {};
}

const std::unordered_map<ConnectionID, std::shared_ptr<Connection>> &DataFlowModel::connections() const
{
    return connections_;
}

// TODO rewrite to unsafe save
QJsonObject DataFlowModel::save() const
{
    qDebug() << "DataNodesModel: save";
    QJsonObject modelJson;

    QJsonArray nodesJsonArray;

    for (auto const &pair : nodes())
    {
        auto const &node = pair.second;

        nodesJsonArray.append(node->save());
    }

    modelJson["nodes"] = nodesJsonArray;

    QJsonDocument document(modelJson);

    return modelJson;
}

QJsonObject DataFlowModel::unsafeSave() const
{
    qDebug() << "DataNodesModel: save";
    QJsonObject modelJson;

    QJsonArray nodesJsonArray;

    for (auto const &pair : nodes())
    {
        auto const &node = pair.second;

        if (node->type() == QtNodes::Item::NodeType::SystemNode)
        {
            nodesJsonArray.push_back(node->unsafeSave());
        }
    }

    modelJson["nodes"] = nodesJsonArray;

    QJsonDocument document(modelJson);

    return modelJson;
}

void DataFlowModel::restore(const QJsonObject &data)
{
    qDebug() << "DataNodesModel: restore";
    QJsonArray nodesJsonArray = data["nodes"].toArray();

    for (const auto &i : nodesJsonArray)
    {
        auto jObj = i.toObject();
        auto name = jObj["name"].toString();
        auto id = QUuid::fromString(jObj["id"].toString());
        auto type = jObj["type"].toInt();
        auto depl = (Item::DeploymentType)jObj["implementation"].toObject()["deploymentType"].toInt();
        // TODO: add deployment type to save\restore
        auto index = addNode(id, (Item::NodeType)type, depl);
        if (index.isValid())
        {
            index.nodeRestore(jObj);
            NameController::Instance()->changeItemCaption(index, index.nodeCaption());
        }
    }

    for (const auto &j : nodesJsonArray)
    {
        auto id = QUuid::fromString(j.toObject()["id"].toString());
        auto index = nodeIndex(id);
        auto parent = nodeIndex(QUuid::fromString(j.toObject()["parent"].toString()));
        setRelation(index, parent);
    }
}

void DataFlowModel::restoreConnections(const QJsonObject &p)
{
    QJsonArray nodesJsonArray = p["nodes"].toArray();
    for (const auto &node : nodesJsonArray)
    {
        auto jObj = node.toObject();
        auto arr = jObj["outConnections"].toArray();
        for (const auto &connection : arr)
        {
            restoreConnection(connection.toObject());
        }
    }
}

void DataFlowModel::clear()
{
    qDebug() << "DataNodesModel: clear";
    for (auto node = std::begin(nodes()); node != std::end(nodes());)
    {
        auto ret = this->removeNodeCommand(nodeIndex(node->first));
        if (!ret.first)
        {
            ++node;
        }
        else
        {
            node = ret.second;
        }
    }
}

void DataFlowModel::restoreConnection(const QJsonObject &connectionJson)
{
    qDebug() << "DataNodesModel: connection restore";
    QUuid nodeInId = QUuid(connectionJson["in_id"].toString());
    QUuid nodeOutId = QUuid(connectionJson["out_id"].toString());
    QUuid nodeServerId = QUuid(connectionJson["server_id"].toString());

    addConnection(nodeIndex(nodeOutId).nodePointer(), nodeIndex(nodeInId).nodePointer(), 
        nodeIndex(nodeInId).nodePointer(), connectionJson);
}

void DataFlowModel::setRelation(const NodeIndex &index, const NodeIndex &parent)
{
    qDebug() << "DataNodesModel: set relation";
    auto found = nodes().find(index.id());
    if (found != nodes().end())
    {
        found->second->setParentNode((nodePointer(parent)));
    }

    // TODO temporary solution for setting deployment type of childs when item
    // with None type added to parent
    for (const auto &it : nodeChilds(index))
    {
        setRelation(it, index);
    }
}

QList<NodeIndex> DataFlowModel::nodes(Item::NodeType type)
{
    QList<NodeIndex> retval;

    for (const auto &[id, node] : nodes_)
    {
        if (node->type() == type)
        {
            retval.append(nodeIndex(id));
        }
    }

    return retval;
}

bool DataFlowModel::empty() const
{
    return nodes_.empty();
}

NodesModel::NodeIterator DataFlowModel::begin() const
{
    return NodeIterator {std::make_unique<DataFlowModelIteratorImpl>(this, nodes().begin())};
}

NodesModel::NodeIterator DataFlowModel::end() const
{
    return NodeIterator {std::make_unique<DataFlowModelIteratorImpl>(this, nodes().end())};
}

void DataFlowModel::nodeRestore(const NodeIndex &index, const QJsonObject &object) const
{
    if (auto node = index.nodePointer())
    {
        node->restore(object);
    }
}

QJsonObject DataFlowModel::nodeSave(const NodeIndex &index) const
{
    if (auto node = index.nodePointer())
    {
        return node->save();
    }
    return {};
}

Connection* DataFlowModel::connectionPointer(const ConnectionID &id) const
{
    auto found = connections().find(id);
    if (found != connections().end())
    {
        return found->second.get();
    }
    return {};
}

void DataFlowModel::nodeSetInfo(const NodeIndex &index, const QJsonObject &info, const QString &filePath)
{
    auto componentImpl = dynamic_cast<ComponentItemImpl*>(nodeImplementation(index));
    if (componentImpl)
    {
        componentImpl->setComponentInfo(info);
        componentImpl->setComponentFilePath(filePath);
        // TODO mauybe fix in future
        NameController::Instance()->changeItemCaption(index, info["caption"].toString());
    }
}
