#pragma once

#include "nodeeditor/nodeeditor_export.h"
#include "common/nodeeditor_globals.hpp"
#include <QJsonObject>
#include <QPixmap>
#include <QUuid>
#include <QtGlobal>
#include <cstddef>
#include <limits>
#include <list>
#include <memory>

class QWidget;

namespace QtNodes
{
class NodesModel;
class ModelAdapter;
class NameController;
class Node;

/**\brief class witch represent information from NodesModel
 */
class NODEEDITOR_EXPORT NodeIndex final
{
    friend NodesModel;
    friend NameController;

public:
    /// Construct an invalid NodeIndex
    NodeIndex();

private:
    /// Regular constructor
    NodeIndex(const QUuid &uuid, void* internalPtr, const NodesModel* model);

public:
    // const void *internalPointer() const;

    /**\return pointer to current Node.
     */
    Node* nodePointer() const;

    /// Get the owning model
    const NodesModel* model() const;

    /// Get the id for the node
    QUuid id() const;

    /**\brief true if uid and model not null
     */
    bool isValid() const;

    // methods from model

    QPixmap nodeIcon() const;

    QString nodeCaption() const;

    QWidget* nodeWidget() const;

    QString nodeValidationMessage() const;

    unsigned int nodePortCount(int pType) const;

    std::list<int> nodeintes(int pType) const;

    QString nodePortCaption(int pType, int pIndex) const;

    std::list<std::pair<NodeIndex, int>> nodePortConnections(QtNodes::Item::PortType pType, int pIndex) const;

    QString nodeTypeIdentifier() const;

    void nodeRestore(const QJsonObject &obj) const;

    QJsonObject nodeSave() const;

    QJsonObject nodeCopy() const;

private:
    QUuid id_;
    void* internalPtr_;
    const NodesModel* model_;
};

inline bool operator==(NodeIndex const &lhs, NodeIndex const &rhs)
{
    return lhs.model() == rhs.model() && lhs.id() == rhs.id();
}

inline bool operator!=(NodeIndex const &lhs, NodeIndex const &rhs)
{
    return !(lhs == rhs);
}

inline bool operator<(NodeIndex const &lhs, NodeIndex const &rhs)
{
    return lhs.id() < rhs.id();
}
} // namespace QtNodes

Q_DECLARE_METATYPE(QtNodes::NodeIndex)
