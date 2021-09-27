#pragma once

#include "lists/Message.hpp"
#include "nodeeditor/nodeeditor_export.h"
#include "Serializable.hpp"
#include <QJsonObject>
#include <QObject>
#include <QPointF>
#include <QUuid>
#include <memory>
#include <set>

namespace QtNodes
{
class Connection;
class NodeImp;
class NodeData;
class NodeIndex;

/**
 * @brief The Node class
 * Basic composite
 */
class NODEEDITOR_EXPORT Node : public QObject, public Serializable
{
Q_OBJECT

public:
    Node(NodeImp* imp, const QUuid &id, Item::NodeType type);

    ~Node() override;

public:
    static QString nodeTypeAsString(Item::NodeType type);
    static Item::NodeType nodeTypeFromString(const QString &typeAsString);

    QUuid id() const;
    NodeImp* imp() const;
    Item::DeploymentType deploymentType();
    QWidget* widget() const;
    QString typeAsString() const;
    Item::NodeType type() const;
    QPixmap icon() const;
    QString caption() const;
    QString validationMessage() const;
    std::list<std::shared_ptr<Connection>> connections(Item::PortType pType, int pIdx) const;
    std::list<std::shared_ptr<Connection>> connections() const;
    virtual std::list<Message> messages(Item::PortType type) const;
    virtual const Node* parentNode() const;
    virtual const QList<Node*> childNodes() const;
    QJsonObject unsafeSave() const;
    QJsonObject save() const override;

    virtual const QList<Node*> childNodesInLowestLevel();
    void addConnection(Item::PortType pType, std::shared_ptr<Connection> conn);
    void removeConnection(Item::PortType pType, std::shared_ptr<Connection> conn);
    void setCaption(const QString &caption);
    virtual void setParentNode(Node* parent);
    void restore(const QJsonObject &obj) override;

signals:
    void updated(const QUuid &uid);

protected:
    virtual void setChildNodes(QList<Node*> childs);
    virtual void removeChildNode(Node* child);

protected:
    QUuid uid_;
    NodeImp* imp_;
    std::map<int, std::list<std::shared_ptr<Connection>>> inConnections_;
    std::map<int, std::list<std::shared_ptr<Connection>>> outConnections_;

    Node* parentNode_;
    QList<Node*> childNodes_;
    Item::NodeType type_;
    Item::DeploymentType deploymentType_;
};
} // namespace QtNodes
