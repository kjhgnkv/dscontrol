// NodesModel.hpp

#pragma once

//
#include "nodeeditor/nodeeditor_export.h"
#include "common/nodeeditor_globals.hpp"

//
#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QPixmap>
#include <QString>
#include <QUuid>
#include <memory>

namespace QtNodes
{
class NodeIndex;
class ModelAdapter;
class NodeIteratorImpl;
class Node;

class Connection;
struct ConnectionID;

/**
 * @brief The NodesModel class
 *
 * Model that contains all elements and connects in project
 */
class NODEEDITOR_EXPORT NodesModel : public QObject
{
Q_OBJECT

public:
    /**
     * @brief NodesModel
     * @param parent
     * default constructor
     */
    explicit NodesModel(QObject* parent = nullptr);
    ~NodesModel() override;

    /**
     * @brief size
     * Get size of node list
     * @return Size
     */
    virtual size_t size() const = 0;
    /**
     * @brief empty
     * Check node list empty
     * @return Is Empty
     */
    virtual bool empty() const = 0;

    /**
     * @brief checkIndex
     * Check index validation
     * @param index Node index
     * @return  true if index isValid and from this model
     */
    virtual bool checkIndex(const NodeIndex &index) const;

public:
    /**
     * @brief nodeIndex
     * Create node index
     * @param ID uniqiue uuid
     * @return new node index
     */
    virtual NodeIndex nodeIndex(const QUuid &ID) const = 0;

    /**
     * @brief nodePointer
     * Get pointer to node
     * @param index node index
     * @return return node pointer
     */
    virtual Node* nodePointer(const NodeIndex &index) const = 0;

    /**
     * @brief nodeTypeIdentifier
     * Get type Id for the node
     * @param index node index
     * @return return type id
     */

    virtual Item::NodeType nodeType(const NodeIndex &index) const = 0;
    virtual Item::DeploymentType nodeDeploymentType(const NodeIndex &index) const = 0;

    /**
     * @brief nodeCaption
     * Get node caption
     * @param index node index
     * @return return QString caption
     */
    virtual QString nodeCaption(NodeIndex const &index) const = 0;
    /**
     * @brief nodeSetCaption
     * Set caption to node
     * @param index node index
     * @param caption node caption
     */
    virtual void nodeSetCaption(const NodeIndex &index, const QString &caption) const = 0;

    /**
     * @brief nodeWidget
     * Get node widget
     * @param index node index
     * @return return QWidhet widget
     */
    virtual QWidget* nodeWidget(NodeIndex const &index) const = 0;

    /**
     * @brief get node parent
     * @param index
     * @return
     */
    virtual NodeIndex nodeParent(const NodeIndex &index) const = 0;

    /**
     * @brief get node childs
     * @param index
     * @return
     */
    virtual QList<NodeIndex> nodeChilds(const NodeIndex &index) const = 0;

    /**
     * @brief get node childs recursily childs
     * @param index
     * @return
     */
    virtual QList<NodeIndex> nodeChildsInLowestLevel(const NodeIndex &index) const = 0;

    /**
     * @brief restore node
     * @param index
     * @param object
     */
    virtual void nodeRestore(const NodeIndex &index, const QJsonObject &object) const = 0;

    /**
     * @brief save node
     * @param index
     * @return
     */
    virtual QJsonObject nodeSave(const NodeIndex &index) const = 0;

    /**
     * @brief copy node
     * @param index
     * @return
     */
    virtual QJsonObject nodeCopy(const NodeIndex &index) const = 0;
    /**
     * @brief paste node
     * @param index
     * @return
     */
    virtual void nodePaste(const QJsonObject &data) = 0;

    /**
     * @brief get connection ptr
     * @param id
     * @return
     */
    virtual Connection* connectionPointer(const ConnectionID &id) const = 0;

    /**
     * @brief get all connections in node
     * @param index
     * @return
     */
    virtual std::list<std::shared_ptr<Connection>> nodeConnections(const NodeIndex &index) const = 0;

    /**
     * @brief add new connection to node
     * @param leftNode
     * @param rightNode
     * @param portIndex
     * @param deviceType
     * @param commName
     * @param msgIdx
     * @param deviceName
     * @return
     */
    virtual bool addConnection(NodeIndex const &leftNode, NodeIndex const &rightNode, NodeIndex const &serverNodeIdx, CommId commId, MsgId msgId) = 0;

    /**
     * @brief remove node
     * @param index
     * @return
     */
    virtual std::pair<bool, std::map<QUuid, std::shared_ptr<Node>>::iterator> removeNode(const NodeIndex &index) = 0;

    virtual std::pair<bool, std::map<QUuid, std::shared_ptr<Node>>::iterator> removeNodeCommand(
    const NodeIndex &index) = 0;

    /**
     * @brief remove nodes
     * @param index
     * @return
     */
    virtual bool removeNodes(const QList<Node*> index) = 0;

    /**
     * @brief remove connection in model and in/out nodes
     * @param id
     * @return
     */
    virtual bool removeConnection(const ConnectionID &id) = 0;

    /**
     * @brief add node with input name and set it in position pos.
     * @return if success - valid index, if failure - {}
     */
    virtual NodeIndex addNode(Item::NodeType type, Item::DeploymentType deployType,
                              const QJsonObject &restore = {}) = 0;

    /**
     * @brief set parent/child relation between nodes
     * @param index
     * @param parent
     */
    virtual void setRelation(const NodeIndex &index, const NodeIndex &parent) = 0;

    /**
     * @brief get all nodes
     * @return
     */
    virtual QList<NodeIndex> nodes(Item::NodeType) = 0;

    /**
     * @brief clear all items in the model
     */
    virtual void clear() = 0;

signals:
    /**
     * @brief emit before node will be removed
     */
    void nodeAboutToBeRemoved(const NodeIndex &index);

    /**
     * @brief emit after node will be removed
     */
    void nodeRemoved();

    /**
     * @brief emit after node was be added
     */
    void nodeAdded(const NodeIndex &index);

    /**
     * @brief emit after node caption was be changed
     * @param id
     */
    void nodeCaptionChanged(const NodeIndex &id);

    /**
     * @brief emit before connection will be removed
     * @param id
     */
    void connectionRemoved(ConnectionID id);

    /**
     * @brief emit after connection was be added
     */
    void connectionAdded(ConnectionID);

    /**
     * @brief emit after node was be updated
     * @param nodeIndex
     */
    void nodeUpdated(const NodeIndex &nodeIndex) const;

    /**
     * @brief emit after connection was be updated
     * @param leftNodeIndex
     * @param leftint
     * @param rightNodeIndex
     * @param rightint
     */
    void updateConnection(const NodeIndex &leftNodeIndex, const NodeIndex &rightNodeIndex,
                          const QtNodes::CommId &rightint);

    /**
     * @brief before deleting model, the model have to emit this signal. We not
     * uses QObject::destroyed because this signal emited after destructors of
     * inheritances classes
     *
     * \warning after emit this signal nothing can not call model!
     */
    void modelDeleted();

public:
    /**
     * @brief forward iterator for DataNodesModel.
     * \warning after insert or remove operation with model, iterator will be
     * invalid. Does not store!
     */
    class NODEEDITOR_EXPORT NodeIterator final
    {
    public:
        explicit NodeIterator();
        ~NodeIterator();

        NodeIterator(const NodeIterator &rhs);
        NodeIterator &operator=(const NodeIterator &rhs);

        NodeIterator &operator++();
        NodeIterator &operator++(int);

        /**
         * @return if iterator does not move and in model was not insert or
         * remove operations return value will be identical
         */
        NodeIndex operator*() const;
        NodeIndex operator->() const;
        bool operator==(const NodeIterator &rhs) const;
        bool operator!=(const NodeIterator &rhs) const;

        explicit NodeIterator(std::unique_ptr<NodeIteratorImpl> impl);

    private:
        std::unique_ptr<NodeIteratorImpl> impl_;
    };

    virtual NodeIterator begin() const = 0;
    virtual NodeIterator end() const = 0;

protected:
    NodeIndex createIndex(const QUuid &id, void* node) const;
    /**
     * @brief create not valid index with id
     */
    NodeIndex createNotValidIndex(const QUuid &id) const;
};

} // namespace QtNodes

namespace std
{
template <>
struct iterator_traits<QtNodes::NodesModel::NodeIterator>
{
    using iterator_category = std::forward_iterator_tag;
    using value_type = QtNodes::NodeIndex;
    using pointer = QtNodes::NodeIndex;
    using reference = QtNodes::NodeIndex;
    using difference_type = std::ptrdiff_t;
};
} // namespace std
