#include "Facade.hpp"

#include "nodes/FlowItemMimeData.hpp"
#include "graphicsComposites/universalGraphicsObject/UniversalGraphicsItem.h"
#include "graphicsComposites/frame/FrameGraphicsItem.hpp"
#include "lists/CommunicationList.hpp"
#include "lists/MessageList.hpp"
#include "models/DataFlowModel.hpp"
#include "models/MimeGenerator.hpp"
#include "models/NodesScene.hpp"
#include "nodes/ComponentNode.hpp"
#include "nodes/Connection.hpp"
#include "nodes/NodeIndex.hpp"
#include "nodes/UniversalGraphicsItemImpl.h"
#include "graphicsComposites/universalGraphicsObject/ports/UniversalPort.h"
#include "scenes/FlowScene.hpp"
#include "singletones/ClipboardController.hpp"
#include "singletones/NameController.hpp"
#include "singletones/UndoController.hpp"
#include "views/FlowView.hpp"
#include "widgets/RenameNodeDialog.h"

#include <QJsonArray>
#include <QMessageBox>
#include <QDebug>

inline void init_res()
{
    // TODO INIT RESOURCE
    Q_INIT_RESOURCE(nodeeditorResources);
}

namespace QtNodes
{
bool operator<(const QPointF &lhs, const QPointF &rhs)
{
    if (lhs.x() < rhs.y() && lhs.y() < rhs.y())
    {
        return true;
    }
    return false;
}

Facade::Facade()
: QObject()
, Serializable()
{
    qDebug() << "Facade::Facade";

    init_res();

    nodesModel_ = new DataFlowModel();
    sceneModel_ = new NodesScene();
    communicationList_ = new CommunicationList {};
    messageList_ = new MessageList(communicationList_);

    sceneModel_->getMainScene()->setFlowModel(nodesModel_);

    //     registry_ = new DataImpRegistry{};
    //     registry_->registerImp<ComponentNode>();
    //     registry_->registerImp<ComputerNode>();
    //     registry_->registerImp<SystemNode>();
    //     registry_->registerImp<SubSystemNode>();
    //
    //     nodesModel_->setRegistry(registry_);

    // connect(fscene.get(), &FlowScene::nodeAdded, this,
    // &Facade::addNodeParent);

    //    messageList_ = new;
    //    communicationList_ = new;

    connect(nodesModel_, &DataFlowModel::nodeAdded, [=](const NodeIndex &index)
    {
        if (auto impl = dynamic_cast<UniversalGraphicsItemImpl*>(index.nodePointer()->imp()); impl && impl->isInstall())
        {
            auto pos = impl->getPos();
            auto parentScene = impl->getParentScene();
            createComposite(index, pos, parentScene);
        }
    });

    connect(nodesModel_, &DataFlowModel::nodeAboutToBeRemoved, this, &Facade::removeNodeGraphics);
    connect(nodesModel_, &DataFlowModel::nodeRemoved, this, &Facade::projectChanged);

    connect(nodesModel_, &DataFlowModel::connectionAdded, this, &Facade::createGraphicsConnect);

    connect(nodesModel_, &DataFlowModel::connectionRemoved, [=](const ConnectionID &id)
    {
        this->disconnectTwoItems(nodesModel_->nodeIndex(id.lNodeID_), nodesModel_->nodeIndex(id.rNodeID_), id.id_);
    });
    connect(nodesModel_, &DataFlowModel::nodeUpdated, this, &Facade::projectChanged);

    // for get node by type:
    // nodesModel_->nodes(id);
}

QJsonObject Facade::save() const
{
    QJsonObject retval;

    retval["model"] = nodesModel_->save();
    retval["scene"] = sceneModel_->save();
    retval["communicationList"] = communicationList_->save();
    retval["messageList"] = messageList_->save();

    qDebug() << "model: " << retval["model"];
    qDebug() << "scene: " << retval["scene"];
    qDebug() << "communicationList: " << retval["communicationList"];
    qDebug() << "messageList: " << retval["messageList"];

    return retval;
}

QJsonObject Facade::unsafeSave() const
{
    QJsonObject retval;

    retval["model"] = nodesModel_->unsafeSave();
    retval["scene"] = sceneModel_->save();
    retval["communicationList"] = communicationList_->save();
    retval["messageList"] = messageList_->save();

    return retval;
}

void Facade::restore(const QJsonObject &p)
{
    communicationList_->restore(p["communicationList"].toObject());
    messageList_->restore(p["messageList"].toObject());

    nodesModel_->restore(p["model"].toObject());
    sceneModel_->restore(p["scene"].toObject());
    nodesModel_->restoreConnections(p["model"].toObject());
    sceneModel_->restoreConnections(p["scene"].toObject());
}

Facade &Facade::Instance()
{
    static Facade instance;
    return instance;
}

NodeIndex Facade::createNode(const Item::NodeType &type, const Item::DeploymentType &deployType,
                             const QJsonObject &info, const QString &filePath)
{
    auto index = nodesModel_->addNode(type, deployType);
    if (!index.isValid())
    {
        showNodesMaxCountMessage(type);
        return index;
    }

    if (type == Item::NodeType::ComponentNode || type == Item::NodeType::ExternalNode)
    {
        nodesModel_->nodeSetInfo(index, info, filePath);
    }

    return index;
}

void Facade::renameNode(const NodeIndex &index)
{
    RenameNodeDialog dialog {index};
    bool hasRenamed = dialog.exec() == QDialog::DialogCode::Accepted;
    if (hasRenamed)
    {
        NameController::Instance()->changeItemCaption(index, dialog.currentName());
        emit nodeRenamed(index);
    }
}

void Facade::createComposite(const NodeIndex &index, const QPointF &pos, const QUuid &parentScene)
{
    if (!index.isValid())
    {
        return;
    }

    auto parentIndex = nodesModel_->nodeIndex(parentScene);
    nodesModel_->setRelation(index, parentIndex);

    auto impl = dynamic_cast<UniversalGraphicsItemImpl*>(nodesModel_->nodePointer(index)->imp());
    if (!impl)
        return;

    impl->setPos(pos);

    auto scene = sceneModel_->getScene(parentScene);
    if (!scene)
    {
        scene = sceneModel_->getMainScene();
    }

    auto newscene = scenePart(index.id(), 2);
    sceneModel_->addScene(index.id(), newscene);
    UniversalGraphicsItem* ugi = new UniversalGraphicsItem(index.id(), impl, newscene.get());

    connectComposite(ugi);

    scene->addUniversalGraphicsItem(ugi, pos);

    emit projectChanged();

    updateItemTitleVisibility(ugi, impl);
    ugi->setViewMode(impl->viewMode());
}

void Facade::updateItemTitleVisibility(UniversalGraphicsItem* ugi, UniversalGraphicsItemImpl* impl)
{
    if (!ugi || !impl)
        return;

    if (!sceneModel_)
        return;

    auto topView = sceneModel_->getTopView();
    if (!topView)
        return;
}

void Facade::connectComposite(UniversalGraphicsItem* item)
{
    connect(item, &UniversalGraphicsItem::doubleClicked, [=]()
    {
        emit this->itemDoubleClicked(item->id());
    });

    connect(item, &UniversalGraphicsItem::geometryAndTitleChanged, this, &Facade::itemInfoChanged);

    connect(item, &UniversalGraphicsItem::itemPropertiesChanged, [=]()
    {
        emit itemPropertiesChanged();
    });

    connect(item, &UniversalGraphicsItem::itemSelectionChanged, [=](bool selected)
    {
        emit this->itemSelected(selected, item->id());
    });
}

void Facade::connectBoundaryFrame(FrameGraphicsItem* item)
{
    connect(item, &FrameGraphicsItem::geometryAndTitleChanged, this, &Facade::itemInfoChanged);
}

void Facade::removeNodeGraphics(const NodeIndex &index)
{
    auto parentIndex = nodesModel_->nodeParent(index);

    switch (nodesModel_->nodeType(index))
    {
        case Item::NodeType::SystemNode:
        {
            sceneModel_->getMainScene()->deleteGraphicsItemCommand(index.id());
            break;
        }
        case Item::NodeType::SubSystemNode:
        case Item::NodeType::ComputerNode:
        case Item::NodeType::ApplicationNode:
        case Item::NodeType::ComponentNode:
        case Item::NodeType::ExternalNode:
        {
            auto scene = parentIndex.isValid() ? sceneModel_->getScene(parentIndex.id()) : sceneModel_->getMainScene();

            scene->deleteGraphicsItemCommand(index.id());

            break;
        }
        default:
        {
            break;
        }
    }

    if (auto scene = sceneModel_->getScene(index.id()))
    {
        sceneModel_->removeScene(index.id());
    }

    emit itemRemoved();
}

void Facade::nodeDropped(const QPointF &pos, const QUuid &source, const QUuid &parent, Item::NodeType type,
                         Item::DeploymentType deployType, const QJsonObject &info, const QString &path)
{
    const auto &srcIndex = nodesModel_->nodeIndex(source);
    const auto &parentIndex = nodesModel_->nodeIndex(parent);
    const auto &srcParentIndex = nodesModel_->nodeParent(srcIndex);

    if (srcIndex.isValid() && parentIndex.isValid() && parentIndex == srcParentIndex)
    {
        return;
    }

    const auto deplType = nodesModel_->nodeDeploymentType(srcIndex);
    if (deplType != Item::DeploymentType::None && deplType != nodesModel_->nodeDeploymentType(parentIndex))
    {
        return;
    }

    auto it = nodesModel_->nodes().find(source);
    NodeIndex index;
    std::list<CutData> data;
    if (it != nodesModel_->nodes().end())
    {
        index = srcIndex;
        const auto &cscene = sceneModel_->getScene(srcParentIndex.id());
        if (cscene)
        {
            data = cut({cscene->graphicsItem(source)});
        }
    }
    else
    {
        index = createNode(type, deployType, info, path);
    }

    if (!data.empty())
    {
        paste(data, parent, pos);
    }
    else if (index.isValid())
    {
        createComposite(index, pos, parent);
    }
}

void Facade::removeComposite(const QUuid &id)
{
    auto index = nodesModel_->nodeIndex(id);

    nodesModel_->removeNode(index);
}

bool Facade::createConnection(const NodeIndex &lNode, const NodeIndex &rNode, 
    const NodeIndex &sNode, CommId commId, MsgId msgId)
{
    return nodesModel_->addConnection(lNode, rNode, sNode, commId, msgId);
}

bool Facade::removeConnection(const NodeIndex &index, const QUuid &uuid)
{
    if (nullptr == nodesModel_)
    {
        return false;
    }

    const auto &conns = nodesModel_->nodeConnections(index);
    auto found = std::find_if(conns.begin(), conns.end(), [index, uuid](const auto &conn)
    {
        const auto &connId = conn->id();
        const auto &indexId = index.id();

        return (connId.lNodeID_ == indexId || connId.rNodeID_ == indexId) && conn->uuid() == uuid;
    });

    if (found == conns.end())
    {
        return false;
    }

    const auto* foundedConnection = found->get();
    if (nullptr == foundedConnection)
    {
        return false;
    }

    const auto* outNode = foundedConnection->getNode(Item::PortType::Out);
    if (nullptr == outNode)
    {
        return false;
    }

    const auto* inNode = foundedConnection->getNode(Item::PortType::In);
    if (nullptr == inNode)
    {
        return false;
    }

    const auto &foundedConnectionId = foundedConnection->id();

    disconnectTwoItems(nodesModel_->nodeIndex(outNode->id()), nodesModel_->nodeIndex(inNode->id()),
                       foundedConnectionId.id_);

    return nodesModel_->removeConnection(foundedConnectionId);
}

bool Facade::removeConnections(const NodeIndex &index)
{
    auto conns = nodesModel_->nodeConnections(index);

    bool ret {false};

    for (const auto &conn : conns)
    {
        ret = removeConnection(index, conn->uuid());
    }

    return ret;
}

bool Facade::removeConnections(const QSet<const Connection*> connections)
{
    for (auto connection : connections)
    {
        auto id = connection->id().lNodeID_;
        Facade::Instance().removeConnections(Facade::Instance().model()->nodeIndex(id));
    }
    return true;
}

std::shared_ptr<FlowScene> Facade::scenePart(const QUuid &uid, qreal scale)
{
    auto node = nodesModel_->nodeIndex(uid);
    auto fscene = sceneModel_->getScene(uid);
    if (fscene == nullptr)
    {
        auto type = nodesModel_->nodeType(node);
        fscene = std::make_shared<FlowScene>(type, uid, node.nodePointer()->imp()->deploymentType());
        fscene->setFlowModel(nodesModel_);
        sceneModel_->addScene(uid, fscene);
        for (auto child : nodesModel_->nodeChilds(node))
        {
            auto scene = sceneModel_->getScene(child.id());
            if (scene == nullptr)
            {
                scene = scenePart(child.id(), scale * 2);
            }
            auto item = dynamic_cast<UniversalGraphicsItemImpl*>(nodesModel_->nodeImplementation(child));
            auto graphicItem = new UniversalGraphicsItem(child.id(), item, scene.get());
            fscene->addUniversalGraphicsItem(graphicItem, {1, 1});
            graphicItem->updateWidget();
        }
    }
    connect(fscene.get(), &FlowScene::nodeDropped, this, &Facade::nodeDropped, Qt::UniqueConnection);
    connect(fscene.get(), &FlowScene::itemsUnselected, [this]()
    {
        emit itemSelected(false, {});
    });
    return fscene;
}

std::shared_ptr<FlowScene> Facade::sceneMain()
{
    auto mainScene = sceneModel_->getMainScene();
    connect(mainScene.get(), &FlowScene::nodeDropped, this, &Facade::nodeDropped, Qt::UniqueConnection);

    return mainScene;
}

DataFlowModel* Facade::model() const
{
    return nodesModel_;
}

NodesScene* Facade::sceneModel() const
{
    return sceneModel_;
}

int Facade::addCommunication()
{
    auto ret = communicationList_->addCommunication();
    return ret;
}

bool Facade::removeCommunication(const int &id)
{
    auto commId = communicationList_->communicationId(id);
    std::vector<ConnectionID> conns;
    for (auto conn : nodesModel_->connections())
    {
        if (conn.first.commId_ == commId)
        {
            conns.push_back(conn.first);
        }
    }
    if (!conns.empty())
    {
        // TODO make custom message box
        const size_t connsCount {conns.size()};
        const QString &sConns = connsCount == 1 ? tr("connection") : tr("connections");
        const QString &text = tr("This communication is used in %1 %2! If you delete it, you will delete all of this."
                                 " Do you want to do this?").arg(connsCount).arg(sConns);
        QMessageBox dialog {{}, tr("Warning!"), text, QMessageBox::Yes | QMessageBox::No};

        switch (dialog.exec())
        {
            case QMessageBox::No:
            {
                return true;
            }
            case QMessageBox::Yes:
            {
                break;
            }
            default:
            {
                return true;
            }
        }

        for (auto conn : conns)
        {
            removeConnection(nodesModel_->nodeIndex(conn.lNodeID_), conn.id_);
        }
    }
    auto ret = communicationList_->removeCommunication(commId);
    return ret;
}

CommunicationList* Facade::communicationList()
{
    return communicationList_;
}

MessageList* Facade::messageList()
{
    return messageList_;
}

QString Facade::generateComponentOwnership(const NodeIndex &index)
{
    QStringList retval;
    retval.push_back(nodesModel_->nodeCaption(index));

    auto parentIdx = nodesModel_->nodeParent(index);

    while (parentIdx.isValid())
    {
        retval.push_back(nodesModel_->nodeCaption(parentIdx));
        parentIdx = nodesModel_->nodeParent(parentIdx);
    }

    return retval.join(" :");
}

void Facade::connectTwoItems(const NodeIndex &idxOut, const NodeIndex &idxIn, int commId, const QUuid &connectionId,
                             int outSide, int inSide)
{
    auto parentOut = nodesModel_->nodeParent(idxOut);
    auto parentIn = nodesModel_->nodeParent(idxIn);

    std::shared_ptr<FlowScene> sceneOut;
    std::shared_ptr<FlowScene> sceneIn;

    // TODO: test and remove commented code.
    // TODO: think about proper definition if node scene is main scene!
    if (!parentOut.isValid())
    {
        sceneOut = sceneModel_->getMainScene();
    }
    else
    {
        sceneOut = sceneModel_->getScene(parentOut.id());
    }

    // TODO: test and remove commented code
    if (!parentIn.isValid())
    {
        sceneIn = sceneModel_->getMainScene();
    }
    else
    {
        sceneIn = sceneModel_->getScene(parentIn.id());
    }
    // TODO: checking scenes to nullptr must be proceeded before?

    auto outItem = dynamic_cast<UniversalGraphicsItem*>(sceneOut->graphicsItem(idxOut.id()));
    auto inItem = dynamic_cast<UniversalGraphicsItem*>(sceneIn->graphicsItem(idxIn.id()));

    QUuid outPortIdx, inPortIdx;
    if (outItem->hasPort(1, commId))
    {
        outPortIdx = outItem->getPort(1, commId)->index();
    }
    else
    {
        outPortIdx = outItem->addPort(UniversalPort::InOutType::Out, commId, outSide);
        sceneOut->addPort(outPortIdx);
    }

    if (inItem->hasPort(0, commId))
    {
        inPortIdx = inItem->getPort(0, commId)->index();
    }
    else
    {
        inPortIdx = inItem->addPort(UniversalPort::InOutType::In, commId, inSide);
        sceneIn->addPort(inPortIdx);
    }

    if (sceneOut == sceneIn)
    {
        sceneOut->createLineConnection(idxOut.id(), outPortIdx, idxIn.id(), inPortIdx, connectionId);
    }
    else
    {
        if (needWaitForOtherSide(idxOut, idxIn))
        {
            // here we cannot create connection to parent port, just wait
            // until other node will be on the same scene to create connection
            // between
            parentOut = idxOut;
        }
        else
        {
            QUuid outParentPortIdx;
            std::shared_ptr<FlowScene> sceneParentOut;

            if (!nodesModel_->nodeParent(parentOut).isValid())
            {
                sceneParentOut = sceneModel_->getMainScene();
            }
            else
            {
                auto parentParentOut = nodesModel_->nodeParent(parentOut);
                sceneParentOut = sceneModel_->getScene(parentParentOut.id());
            }
            auto outParentItem = dynamic_cast<UniversalGraphicsItem*>(sceneParentOut->graphicsItem(parentOut.id()));
            if (outParentItem->hasPort(1, commId))
            {
                outParentPortIdx = outParentItem->getPort(UniversalPort::InOutType::Out, commId)->index();
            }
            else
            {
                outParentPortIdx = outParentItem->addPort(UniversalPort::InOutType::Out, commId, outSide);
                sceneParentOut->addPort(outParentPortIdx);
            }
            sceneOut->createLineConnection(idxOut.id(), outPortIdx,
                                           outParentItem->getPort(outParentPortIdx)->getPortProjectionItem(),
                                           connectionId);
        }
        if (needWaitForOtherSide(idxIn, idxOut))
        {
            // here we cannot create connection to parent port, just wait
            // until other node will be on the same scene to create connection
            // between
            parentIn = idxIn;
        }
        else
        {
            QUuid inParentPortIdx;
            std::shared_ptr<FlowScene> sceneParentIn;

            if (!nodesModel_->nodeParent(parentIn).isValid())
            {
                sceneParentIn = sceneModel_->getMainScene();
            }
            else
            {
                auto parentParentIn = nodesModel_->nodeParent(parentIn);
                sceneParentIn = sceneModel_->getScene(parentParentIn.id());
            }
            auto inParentItem = dynamic_cast<UniversalGraphicsItem*>(sceneParentIn->graphicsItem(parentIn.id()));
            if (inParentItem->hasPort(0, commId))
            {
                inParentPortIdx = inParentItem->getPort(0, commId)->index();
            }
            else
            {
                inParentPortIdx = inParentItem->addPort(UniversalPort::InOutType::In, commId, inSide);
                sceneParentIn->addPort(inParentPortIdx);
            }

            sceneIn->createLineConnection(idxIn.id(), inPortIdx,
                                          inParentItem->getPort(inParentPortIdx)->getPortProjectionItem(),
                                          connectionId);
        }
        connectTwoItems(parentOut, parentIn, commId, connectionId, outSide, inSide);
    }
}

void Facade::disconnectTwoItems(const NodeIndex &idxOut, const NodeIndex &idxIn, const QUuid &connectionId)
{
    NodeIndex parentOut;
    NodeIndex parentIn;
    if (idxOut.isValid())
    {
        parentOut = nodesModel_->nodeParent(idxOut);
        std::shared_ptr<FlowScene> sceneOut;

        if (!parentOut.isValid())
        {
            sceneOut = sceneModel_->getMainScene();
        }
        else
        {
            sceneOut = sceneModel_->getScene(parentOut.id());
        }

        if (sceneOut)
        {
            sceneOut->removeLineConnection(idxOut.id(), connectionId);
        }
    }

    if (idxIn.isValid())
    {
        parentIn = nodesModel_->nodeParent(idxIn);
        std::shared_ptr<FlowScene> sceneIn;

        if (!parentIn.isValid())
        {
            sceneIn = sceneModel_->getMainScene();
        }
        else
        {
            sceneIn = sceneModel_->getScene(parentIn.id());
        }

        if (sceneIn)
        {
            sceneIn->removeLineConnection(idxIn.id(), connectionId);
        }
    }
    if (!idxOut.isValid() && !idxIn.isValid())
    {
        return;
    }

    disconnectTwoItems(parentOut, parentIn, connectionId);
}

void Facade::createGraphicsConnect(ConnectionID id)
{
    connect(communicationList_, &CommunicationList::communicationUpdated, [=](int commId, QVector<int> roles)
    {
        auto connections = nodesModel_->connections();
        auto conn = connections.at(id);
        if (conn->id().commId_ == commId)
        {
            QVariantList updates;
            QVector<int> updatesRoles;

            for (const auto &role : roles)
            {
                switch (role)
                {
                    case CommunicationList::Columns::DeviceType:
                    {
                        conn->setDeviceType(communicationList_->communicationDeviceType(commId));
                        break;
                    }
                    case CommunicationList::Columns::DeviceName:
                    {
                        conn->setDeviceName(communicationList_->communicationDeviceName(commId));
                        break;
                    }
                    case CommunicationList::Columns::CommName:
                    {
                        auto value = communicationList_->communicationName(commId);
                        conn->setCommName(value);

                        break;
                    }
                    case CommunicationList::Columns::Id:
                    {
                        conn->setCommId(commId);
                        break;
                    }
                }
            }
        }
    });

    auto sides = this->getPortsSides(nodesModel_->nodeIndex(id.lNodeID_), nodesModel_->nodeIndex(id.rNodeID_));
    this->connectTwoItems(nodesModel_->nodeIndex(id.lNodeID_), nodesModel_->nodeIndex(id.rNodeID_), id.commId_, id.id_,
                          sides.first, sides.second);

    // Update connection arrows visibility, if connection was created for items on inner scenes
    if (sceneModel_)
    {
        auto topView = sceneModel_->getTopView();
        if (topView)
        {
            topView->showTopLevelConnections(false);
        }
    }
}

bool Facade::canCopy(QList<QGraphicsItem*> items)
{
    auto size = items.size();
    if (size < 1)
    {
        return false;
    }

    for (const auto &item : items)
    {
        if (item->type() == static_cast<int>(Arrows::ArrowType::Connection))
        {
            return false;
        }
    }

    return true;
}

bool Facade::canCut(QList<QGraphicsItem*> items)
{
    return canCopy(items);
}

bool Facade::canPaste(QMimeData* data, const QUuid &sceneId)
{
    auto json = reinterpret_cast<FlowItemMimeData*>(data)->info();
    auto nodes = json["nodes"].toArray();

    if (!canPasteByCount(nodes))
    {
        return false;
    }

    auto targetType = sceneModel_->getScene(sceneId)->type();
    bool retval = true;

    for (auto node : nodes)
    {
        if (!retval)
        {
            break;
        }
        auto model = node.toObject()["model"].toObject();
        if (model.isEmpty())
        {
            continue;
        }
        auto type = static_cast<Item::NodeType>(model["type"].toInt());
        if (type == Item::NodeType::Text || type == Item::NodeType::Frame)
        {
            continue;
        }

        auto deploymentType = static_cast<Item::DeploymentType>(model["deploymentType"].toInt());
        retval = canPasteHelper(targetType, type, deploymentType);
    }

    return retval;
}

bool Facade::canPaste(std::list<CutData> data, const QUuid &sceneId)
{
    auto targetType = sceneModel_->getScene(sceneId)->type();
    bool retval = true;
    for (const auto &[index, connsToRestore, item] : data)
    {
        if (!retval)
        {
            break;
        }
        auto type = static_cast<Item::NodeType>(item->type());
        auto deploymentType = nodesModel_->nodeDeploymentType(nodesModel_->nodeIndex(item->id()));
        retval = canPasteHelper(targetType, type, deploymentType);
    }

    return retval;
}

QMimeData* Facade::copy()
{
    QMimeData* retval = nullptr;
    auto selectedItems = sceneModel_->selectedItems();
    if (canCopy(selectedItems))
    {
        retval = MimeGenerator::generateCopyMime(selectedItems);
        sceneModel_->clearSelection();
    }

    return retval;
}

std::list<CutData> Facade::cut(QList<QGraphicsItem*> items)
{
    if (!canCopy(items))
    {
        return {};
    }
    std::list<CutData> retval;
    std::map<QUuid, ConnectionID> connsToRestore;
    NodeIndex index {};

    for (const auto &obj : items)
    {
        auto item = dynamic_cast<UniversalSelectableItem*>(obj);
        if (item)
        {
            auto sourceScene = item->flowScene();

            if (sourceScene)
            {
                index = nodesModel_->nodeIndex(item->id());
                if (index.isValid())
                {
                    connsToRestore = removeNodeConnections(index);
                }
                sourceScene->removeGraphicsItem(item);
            }
        }

        retval.push_back({index, connsToRestore, item});
    }

    return retval;
}

std::list<CutData> Facade::cut()
{
    auto items = sceneModel_->selectedItems();
    return cut(items);
}

void Facade::paste(QMimeData* data, const QUuid &sceneId, QPointF pos)
{
    if (!data || sceneId.isNull() || !canPaste(data, sceneId))
    {
        return;
    }
    // maybe move to dataflowmodel
    UndoController::Instance().begin("Paste");

    auto json = reinterpret_cast<FlowItemMimeData*>(data)->info();
    auto nodes = json["nodes"].toArray();
    auto rectJson = json["rect"].toObject();
    QRectF rect {
    rectJson["x"].toDouble(), rectJson["y"].toDouble(), rectJson["width"].toDouble(), rectJson["height"].toDouble()
    };
    auto size = nodes.size();

    for (auto node : nodes)
    {
        if (auto model = node.toObject()["model"].toObject(); !model.isEmpty())
        {
            auto impl = model["implementation"].toObject();
            impl["parentScene"] = sceneId.toString();

            QPointF basePos {impl["pos"].toObject()["x"].toDouble(), impl["pos"].toObject()["y"].toDouble()};

            QJsonObject tmp = {};
            if (size == 1)
            {
                tmp = QJsonObject {
                {  "x", pos.x()}
                , {"y", pos.y()}
                };
            }
            else
            {
                tmp = QJsonObject {
                {  "x", basePos.x() - rect.topLeft().x() + pos.x()}
                , {"y", basePos.y() - rect.topLeft().y() + pos.y()}
                };
            }
            impl["pos"] = tmp;
            model["implementation"] = impl;

            Facade::Instance().model()->nodePaste(model);
        }
        else if (auto view = node.toObject()["view"].toObject(); !view.isEmpty())
        {
            QPointF basePos {view["pos"].toObject()["x"].toDouble(), view["pos"].toObject()["y"].toDouble()};

            QJsonObject tmp {
            {  "x", basePos.x() - rect.topLeft().x() + pos.x()}
            , {"y", basePos.y() - rect.topLeft().y() + pos.y()}
            };
            view["pos"] = tmp;
            Facade::Instance().sceneModel()->getScene(sceneId)->paste(view);
        }
    }
    UndoController::Instance().end();
}

void Facade::paste(std::list<CutData> data, const QUuid &sceneId, QPointF pos)
{
    if (data.empty() || sceneId.isNull() || !canPaste(data, sceneId))
    {
        return;
    }

    QRectF itemsRect {};
    for (const auto &[index, connsToRestore, item] : data)
    {
        itemsRect |= item->sceneBoundingRect();
    }
    for (const auto &[index, connsToRestore, item] : data)
    {
        std::shared_ptr<FlowScene> scene = nullptr;
        if (sceneId.isNull())
        {
            scene = sceneModel_->getMainScene();
        }
        else
        {
            scene = sceneModel_->getScene(sceneId);
        }
        scene->addGraphicsItem(item, item->pos() - itemsRect.topLeft() + pos);
    }

    for (const auto &[index, connsToRestore, item] : data)
    {
        if (index.isValid())
        {
            auto parentIdx = nodesModel_->nodeIndex(sceneId);
            nodesModel_->setRelation(index, parentIdx);
            restoreNodeConnections(connsToRestore);
        }
    }
    emit projectChanged();
}

void Facade::clear()
{
    qDebug() << "Facade::clear";

    nodesModel_->clear();
    sceneModel_->clear();
    communicationList_->clear();
    messageList_->clear();
    NameController::Instance()->clear();
    UndoController::Instance().clear();
}

std::pair<int, int> Facade::getPortsSides(const NodeIndex &idxOut, const NodeIndex &idxIn)
{
    auto parentOut = nodesModel_->nodeParent(idxOut);
    auto parentIn = nodesModel_->nodeParent(idxIn);

    std::shared_ptr<FlowScene> sceneOut;
    std::shared_ptr<FlowScene> sceneIn;

    if (!parentOut.isValid())
    {
        sceneOut = sceneModel_->getMainScene();
    }
    else
    {
        sceneOut = sceneModel_->getScene(parentOut.id());
    }

    if (!parentIn.isValid())
    {
        sceneIn = sceneModel_->getMainScene();
    }
    else
    {
        sceneIn = sceneModel_->getScene(parentIn.id());
    }

    if (sceneOut == sceneIn)
    {
        int outSide = UniversalPort::Top;
        int inSide = UniversalPort::Top;

        auto outItem = sceneOut->graphicsItem(idxOut.id());
        auto inItem = sceneOut->graphicsItem(idxIn.id());

        auto outPos = outItem->scenePos();
        auto inPos = inItem->scenePos();

        auto outHeight = outItem->boundingRect().height();
        auto inHeight = inItem->boundingRect().height();

        auto outWidth = outItem->boundingRect().width();
        auto inWidth = inItem->boundingRect().width();

        // out-node is completely to the left than in-node
        if ((outPos.rx() + outWidth) < inPos.rx())
        {
            // out-node is completely to the top than in-node
            if (inPos.ry() > (outPos.ry() + outHeight))
            {
                // Y-difference is greater than X
                if ((inPos.rx() - outPos.rx() + outWidth) < (inPos.ry() - outPos.ry() + outHeight))
                {
                    outSide = UniversalPort::Bottom;
                    inSide = UniversalPort::Top;
                }
                else
                {
                    outSide = UniversalPort::Right;
                    inSide = UniversalPort::Left;
                }
            } // in-node is completely to the top than out-node
            else if (outPos.ry() > (inPos.ry() + inHeight))
            {
                // Y-difference is greater than X
                if ((outPos.rx() - inPos.rx() + inWidth) < (outPos.ry() - inPos.ry() + inHeight))
                {
                    outSide = UniversalPort::Top;
                    inSide = UniversalPort::Bottom;
                }
                else
                {
                    outSide = UniversalPort::Right;
                    inSide = UniversalPort::Left;
                }
            } // side-by-side
            else
            {
                outSide = UniversalPort::Right;
                inSide = UniversalPort::Left;
            }
        } // in-node is completely to the left than out-node
        else if ((inPos.rx() + inWidth) < outPos.rx())
        {
            // out-node is completely to the top than in-node
            if (inPos.ry() > (outPos.ry() + outHeight))
            {
                // Y-difference is greater than X
                if ((outPos.rx() - inPos.rx() + inWidth) < (inPos.ry() - outPos.ry() + outHeight))
                {
                    outSide = UniversalPort::Bottom;
                    inSide = UniversalPort::Top;
                }
                else
                {
                    outSide = UniversalPort::Left;
                    inSide = UniversalPort::Right;
                }
            } // in-node is completely to the top than out-node
            else if (outPos.ry() > (inPos.ry() + inHeight))
            {
                // Y-difference is greater than X
                if ((outPos.rx() - inPos.rx() + inWidth) < (outPos.ry() - inPos.ry() + inHeight))
                {
                    outSide = UniversalPort::Top;
                    inSide = UniversalPort::Bottom;
                }
                else
                {
                    outSide = UniversalPort::Left;
                    inSide = UniversalPort::Right;
                }
            } // side-by-side
            else
            {
                outSide = UniversalPort::Left;
                inSide = UniversalPort::Right;
            }
        } // side-by-side
        else
        {
            // out-node is to the top than in-node
            if (outPos.ry() < inPos.ry())
            {
                outSide = UniversalPort::Bottom;
                inSide = UniversalPort::Top;
            }
            else
            {
                outSide = UniversalPort::Top;
                inSide = UniversalPort::Bottom;
            }
        }

        return std::make_pair(outSide, inSide);
    }
    else
    {
        if (needWaitForOtherSide(idxOut, idxIn))
        {
            // just wait until other node will be on the same scene
            parentOut = idxOut;
        }
        if (needWaitForOtherSide(idxIn, idxOut))
        {
            // just wait until other node will be on the same scene
            parentIn = idxIn;
        }

        return getPortsSides(parentOut, parentIn);
    }
}

bool Facade::needWaitForOtherSide(const NodeIndex &idxMe, const NodeIndex &idxOther)
{
    std::shared_ptr<FlowScene> sceneMe;
    std::shared_ptr<FlowScene> sceneOther;

    NodeIndex nodeOther = idxOther;

    auto parentMe = nodesModel_->nodeParent(idxMe);

    if (!parentMe.isValid())
    {
        sceneMe = sceneModel_->getMainScene();
    }
    else
    {
        sceneMe = sceneModel_->getScene(parentMe.id());
    }

    NodeIndex parentOther;

    do
    {
        parentOther = nodesModel_->nodeParent(nodeOther);
        if (!parentOther.isValid())
        {
            sceneOther = sceneModel_->getMainScene();
        }
        else
        {
            sceneOther = sceneModel_->getScene(parentOther.id());
        }

        if (sceneMe == sceneOther)
        {
            return true;
        }
        else
        {
            nodeOther = parentOther;
        }

    } while (parentOther.isValid());

    return false;
}

std::map<QUuid, ConnectionID> Facade::removeNodeConnections(const NodeIndex &index)
{
    std::map<QUuid, ConnectionID> conns;

    auto connections = nodesModel_->nodeConnectionsInDepth(index);

    for (auto node : connections)
    {
        for (auto conn : node.second)
        {
            conns[conn->uuid()] = conn->id();
        }
    }

    for (auto conn : conns)
    {
        removeConnection(nodesModel_->nodeIndex(conn.second.lNodeID_), conn.first);
    }
    return conns;
}

void Facade::restoreNodeConnections(std::map<QUuid, ConnectionID> conns)
{
    for (auto conn : conns)
    {
        createConnection(nodesModel_->nodeIndex(conn.second.lNodeID_), nodesModel_->nodeIndex(conn.second.rNodeID_),
                         nodesModel_->nodeIndex(conn.second.sNodeID_), conn.second.commId_, conn.second.msgId_);
    }
}

NodesCountMap Facade::createNodesCountMap(const QJsonArray &nodes) const
{
    NodesCountMap ret;
    const size_t minNodesCount {1};
    for (auto node : nodes)
    {
        auto model = node.toObject()["model"].toObject();
        if (model.isEmpty())
        {
            continue;
        }
        auto nodeType = static_cast<Item::NodeType>(model["type"].toInt());
        bool isInserted = ret.insert({nodeType, minNodesCount}).second;
        if (!isInserted)
        {
            ++ret.at(nodeType);
        }
    }
    return ret;
}

bool Facade::canPasteByCount(const QJsonArray &nodes) const
{
    if (nullptr == nodesModel_)
    {
        return false;
    }

    const auto &nodesCountMap = createNodesCountMap(nodes);
    const auto &canAddNodesRet = nodesModel_->canAddNodes(nodesCountMap);
    if (!canAddNodesRet.first)
    {
        showNodesMaxCountMessage(canAddNodesRet.second);
    }

    return canAddNodesRet.first;
}

bool Facade::canPasteHelper(Item::NodeType targetType, Item::NodeType type, Item::DeploymentType deploymentType) const
{
    switch (targetType)
    {
        case Item::NodeType::MainScene:
        {
            return (type == Item::NodeType::SystemNode || deploymentType == Item::DeploymentType::External);
        }
        case Item::NodeType::SystemNode:
        {
            return (type == Item::NodeType::SubSystemNode || deploymentType == Item::DeploymentType::External);
        }
        case Item::NodeType::SubSystemNode:
        {
            return (type == Item::NodeType::ComputerNode || deploymentType == Item::DeploymentType::External);
        }
        case Item::NodeType::ComputerNode:
        {
            return (type == Item::NodeType::ApplicationNode || deploymentType == Item::DeploymentType::External);
        }
        case Item::NodeType::ApplicationNode:
        {
            return (type == Item::NodeType::ComponentNode || deploymentType == Item::DeploymentType::External);
        }
        case Item::NodeType::ComponentNode:
        {
            return true;
        }
        default:
        {
            return false;
        }
    }
}

void Facade::showNodesMaxCountMessage(Item::NodeType type) const
{
    const QString &title {"The maximum allowable number of elements in the project is exceeded."};
    const auto &message = QString {"You have exceeded the maximum number of %1s in the project"}.arg(
    Node::nodeTypeAsString(type));
    QMessageBox::warning(nullptr, title, message);
}

void Facade::extractCode(const QString &name)
{
    emit extractComponentCode(name);
}

} // namespace QtNodes
