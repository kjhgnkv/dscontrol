#include "Node.hpp"
#include "Connection.hpp"
#include "NodeImp.hpp"
#include "NodeIndex.hpp"
#include <QDebug>
#include <QJsonArray>
#include <QtCore/QObject>

static std::set<QUuid> voidList;

using namespace QtNodes;

Node::Node(NodeImp* imp, const QUuid &id, Item::NodeType type)
: uid_ {id}
, imp_ {imp}
, parentNode_ {nullptr}
, childNodes_ {}
, type_ {type}
{
    if (imp_)
    {
        connect(imp_, &NodeImp::nodeUpdated, this, [this]()
        {
            emit updated(this->id());
        });
    }
}

Node::~Node()
{
    if (parentNode_)
    {
        parentNode_->removeChildNode(this);
    }
}

QWidget* Node::widget() const
{
    if (imp_)
    {
        return imp_->embeddedWidget();
    }
    return {};
}

QString Node::typeAsString() const
{
    return nodeTypeAsString(type_);
}

Item::NodeType Node::type() const
{
    return type_;
}

QString Node::caption() const
{
    if (imp_)
    {
        return imp_->caption();
    }
    return {};
}

void Node::setCaption(const QString &caption)
{
    if (imp_)
    {
        imp_->setCaption(caption);
    }
}

QPixmap Node::icon() const
{
    if (imp_)
    {
        return imp_->icon();
    }
    return {};
}

void Node::addConnection(Item::PortType pType, std::shared_ptr<Connection> conn)
{
    auto commId = conn->id().commId_;
    if (pType == Item::PortType::In)
    {
        auto found = inConnections_.find(commId);
        if (found != inConnections_.end())
        {
            found->second.push_back(conn);
        }
        else
        {
            inConnections_.insert({commId, {conn}});
        }
    }
    else
    {
        auto found = outConnections_.find(commId);
        if (found != outConnections_.end())
        {
            found->second.push_back(conn);
        }
        else
        {
            outConnections_.insert({commId, {conn}});
        }
    }
}

void Node::removeConnection(Item::PortType pType, std::shared_ptr<Connection> conn)
{
    auto commId = conn->id().commId_;
    if (pType == Item::PortType::In)
    {
        auto found = inConnections_.find(commId);
        if (found != inConnections_.end())
        {
            auto findInLst = std::find(found->second.begin(), found->second.end(), conn);
            if (findInLst != found->second.end())
            {
                found->second.erase(findInLst);
                if (found->second.empty())
                {
                    inConnections_.erase(found);
                }
            }
        }
    }
    else
    {
        auto found = outConnections_.find(commId);
        if (found != outConnections_.end())
        {
            auto findInLst = std::find(found->second.begin(), found->second.end(), conn);
            if (findInLst != found->second.end())
            {
                found->second.erase(findInLst);
                if (found->second.empty())
                {
                    outConnections_.erase(found);
                }
            }
        }
    }
}

QJsonObject Node::save() const
{
    if (imp_)
    {
        QJsonObject nodeJson;

        nodeJson["name"] = imp_->caption();
        nodeJson["id"] = id().toString();
        nodeJson["type"] = (int)type_;
        if (parentNode_)
            nodeJson["parent"] = parentNode_->id().toString();
        else
        {
            nodeJson["parent"] = QUuid().toString();
        }

        QJsonArray arr;
        QJsonObject tmp;

        for (const auto &child : childNodes_)
        {
            tmp["id"] = child->id().toString();
            arr.push_back(tmp);
        }

        nodeJson["childs"] = tmp;

        nodeJson["implementation"] = imp_->save();

        // only out connections
        QJsonArray outConnections;
        for (const auto &i : outConnections_)
        {
            for (const auto &j : i.second)
            {
                outConnections.push_back(j->save());
            }
        }

        QJsonArray inConnections;
        for (const auto &i : inConnections_)
        {
            for (const auto &j : i.second)
            {
                inConnections.push_back(j->save());
            }
        }

        if (!outConnections.isEmpty())
        {
            nodeJson["outConnections"] = outConnections;
        }
        if (!inConnections.isEmpty())
        {
            nodeJson["inConnections"] = inConnections;
        }

        return nodeJson;
    }
    return {};
}

QJsonObject Node::unsafeSave() const
{
    if (imp_)
    {
        QJsonObject nodeJson;

        nodeJson["name"] = imp_->caption();
        nodeJson["id"] = id().toString();
        nodeJson["type"] = (int)type_;
        if (parentNode_)
            nodeJson["parent"] = parentNode_->save();
        else
        {
            nodeJson["parent"] = {};
        }

        QJsonArray arr;

        for (const auto &child : childNodes_)
        {
            arr.push_back(child->unsafeSave());
        }

        nodeJson["childs"] = arr;

        nodeJson["implementation"] = imp_->save();

        // only out connections
        QJsonArray outConnections;
        for (const auto &i : outConnections_)
        {
            for (const auto &j : i.second)
            {
                outConnections.push_back(j->save());
            }
        }

        QJsonArray inConnections;
        for (const auto &i : inConnections_)
        {
            for (const auto &j : i.second)
            {
                inConnections.push_back(j->save());
            }
        }

        if (!outConnections.isEmpty())
        {
            nodeJson["outConnections"] = outConnections;
        }
        if (!inConnections.isEmpty())
        {
            nodeJson["inConnections"] = inConnections;
        }

        return nodeJson;
    }
    return {};
}

QUuid Node::id() const
{
    if (imp_)
    {
        return uid_;
    }
    return {};
}

QString Node::nodeTypeAsString(Item::NodeType type)
{
    switch (type)
    {
        case Item::NodeType::None:
        {
            return "None";
        }
        case Item::NodeType::MainScene:
        {
            return "MainScene";
        }
        case Item::NodeType::SystemNode:
        {
            return "SystemNode";
        }
        case Item::NodeType::SubSystemNode:
        {
            return "SubSystemNode";
        }
        case Item::NodeType::ComputerNode:
        {
            return "ComputerNode";
        }
        case Item::NodeType::ApplicationNode:
        {
            return "ApplicationNode";
        }
        case Item::NodeType::ComponentNode:
        {
            return "ComponentNode";
        }
        case Item::NodeType::Node:
        {
            return "Node";
        }
        case Item::NodeType::Note:
        {
            return "Note";
        }
        case Item::NodeType::Port:
        {
            return "Port";
        }
        case Item::NodeType::Text:
        {
            return "Text";
        }
        case Item::NodeType::Frame:
        {
            return "Frame";
        }
        case Item::NodeType::Connection:
        {
            return "Connection";
        }
        case Item::NodeType::ExternalNode:
        {
            return "External";
        }
        case Item::NodeType::OtherType:
        {
            return "OtherType";
        }
        default:
        {
            return "Unknown";
        }
    }
}

Item::NodeType Node::nodeTypeFromString(const QString &typeAsString)
{
    if (typeAsString == "SystemNode")
    {
        return Item::NodeType::SystemNode;
    }
    if (typeAsString == "SubsystemNode")
    {
        return Item::NodeType::SubSystemNode;
    }
    if (typeAsString == "DeviceNode" || typeAsString == "ComputerNode")
    {
        return Item::NodeType::ComputerNode;
    }
    if (typeAsString == "ApplicationNode")
    {
        return Item::NodeType::ApplicationNode;
    }
    if (typeAsString == "ComponentNode")
    {
        return Item::NodeType::ComponentNode;
    }
    if (typeAsString == "ExternalNode")
    {
        return Item::NodeType::ExternalNode;
    }
    return Item::NodeType::OtherType;
}

NodeImp* Node::imp() const
{
    return imp_;
}

Item::DeploymentType Node::deploymentType()
{
    if (imp_)
    {
        return imp_->deploymentType();
    }

    return {};
}

std::list<std::shared_ptr<Connection>> Node::connections(Item::PortType pType, int idx) const
{
    if (pType == Item::PortType::In)
    {
        auto found = inConnections_.find(idx);
        if (found != inConnections_.end())
        {
            return found->second;
        }
    }
    else
    {
        auto found = outConnections_.find(idx);
        if (found != outConnections_.end())
        {
            return found->second;
        }
    }

    return {};
}

std::list<std::shared_ptr<Connection>> Node::connections() const
{
    std::list<std::shared_ptr<Connection>> ret;

    for (const auto &it : outConnections_)
    {
        ret.insert(ret.end(), it.second.begin(), it.second.end());
    }

    for (const auto &it : inConnections_)
    {
        ret.insert(ret.end(), it.second.begin(), it.second.end());
    }

    return ret;
}

void Node::setParentNode(Node* parentNode)
{
    if (parentNode_)
    {
        parentNode_->removeChildNode(this);
    }
    if (parentNode && parentNode != this)
    {
        parentNode_ = parentNode;
        parentNode->setChildNodes({this});
        imp_->setDeploymentType(parentNode_->deploymentType());
    }
    else
    {
        parentNode_ = nullptr;
    }

    emit updated(this->id());
}

void Node::setChildNodes(QList<Node*> childs)
{
    for (const auto &i : childs)
    {
        if (!childNodes_.contains(i))
        {
            childNodes_.append(i);
        }
    }
}

void Node::removeChildNode(Node* child)
{
    if (!child)
    {
        return;
    }
    if (childNodes_.contains(child))
    {
        childNodes_.removeOne(child);
    }
}

const Node* Node::parentNode() const
{
    return parentNode_;
}

const QList<Node*> Node::childNodes() const
{
    return {};
}

const QList<Node*> Node::childNodesInLowestLevel()
{
    return {};
}

std::list<Message> Node::messages(Item::PortType) const
{
    return {};
}

void Node::restore(const QJsonObject &obj)
{
    uid_ = QUuid::fromString(obj["id"].toString());
    type_ = static_cast<Item::NodeType>(obj["type"].toInt());
    if (imp_)
    {
        imp_->restore(obj["implementation"].toObject());
    }
}
