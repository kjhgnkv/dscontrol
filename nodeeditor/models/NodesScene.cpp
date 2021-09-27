#include "NodesScene.hpp"

#include "QDebug"
#include "common/nodeeditor_globals.hpp"
#include "nodes/UniversalGraphicsItemImpl.h"
#include "scenes/FlowScene.hpp"
#include "singletones/Facade.hpp"

#include <QJsonArray>
#include "views/FlowView.hpp"

namespace QtNodes
{
NodesScene::NodesScene()
: QObject(nullptr)
, Serializable()
, topView_ {nullptr}
, selectedScene_ {nullptr}
{
    qDebug() << "NodesScene: constructor";
    mainScene_ = std::make_shared<FlowScene>(Item::NodeType::MainScene, QUuid::createUuid(),
                                             Item::DeploymentType::Main);

    topScene_ = mainScene_.get();

    connect(mainScene_.get(), &QGraphicsScene::selectionChanged, this, &NodesScene::sceneSelectionChangedSlot);
}

NodesScene::~NodesScene()
{
    qDebug() << "NodesScene: destructor";
    scenesMap_.clear();
}

void NodesScene::addScene(const QUuid &uid, std::shared_ptr<FlowScene> newScene)
{
    connect(newScene.get(), &QGraphicsScene::selectionChanged, this, &NodesScene::sceneSelectionChangedSlot);

    auto[it, isInserted] = scenesMap_.insert({uid, newScene});
    if (isInserted)
    {
        newScene->clearSelection();
    }
}

void NodesScene::removeScene(const QUuid &uid)
{
    auto removingScene = scenesMap_.find(uid);
    if (scenesMap_.end() != removingScene)
    {
        disconnect(removingScene->second.get(), &QGraphicsScene::selectionChanged, this,
                   &NodesScene::sceneSelectionChangedSlot);
        if (removingScene->second.get() == selectedScene_)
        {
            selectedScene_ = nullptr;
        }
        scenesMap_.erase(removingScene);
    }
}

std::shared_ptr<FlowScene> NodesScene::getScene(const QUuid &uid)
{
    if (scenesMap_.find(uid) != scenesMap_.end())
    {
        return scenesMap_.at(uid);
    }
    else if (mainScene_->id() == uid)
    {
        return mainScene_;
    }
    else
    {
        return nullptr;
    }
}

void NodesScene::setMainScene(std::shared_ptr<FlowScene> scene)
{
    if (nullptr != mainScene_)
    {
        disconnect(mainScene_.get(), &QGraphicsScene::selectionChanged, this, &NodesScene::sceneSelectionChangedSlot);

        mainScene_.reset();
    }

    mainScene_ = scene;
    mainScene_->setFlowModel(Facade::Instance().model());
    connect(mainScene_.get(), &QGraphicsScene::selectionChanged, this, &NodesScene::sceneSelectionChangedSlot);
}

std::shared_ptr<FlowScene> NodesScene::getMainScene() const
{
    return mainScene_;
}

void NodesScene::setTopScene(FlowScene* scene)
{
    const auto old = topScene_;
    topScene_ = scene;
    emit topSceneChanged(old);
}

FlowScene* NodesScene::getTopScene() const
{
    return topScene_;
}

void NodesScene::setTopView(FlowView* view)
{
    const auto old = topView_;

    if (view)
    {
        topView_ = view;
        if (auto scene = view->flowScene())
        {
            setTopScene(scene);
        }
        else
        {
            setTopScene(nullptr);
        }
    }
    else
    {
        topView_ = nullptr;
        setTopScene(nullptr);
    }

    emit topViewChanged(old);
}

FlowView* NodesScene::getTopView() const
{
    return topView_;
}

std::shared_ptr<FlowScene> NodesScene::getSelectedScene() const
{
    for (auto scene : QtNodes::Facade::Instance().sceneModel()->allScenes())
    {
        if (scene->selectedItems().size() > 0)
        {
            return scene;
        }
    }

    return nullptr;
}

void NodesScene::clear()
{
    setTopView(nullptr);

    setMainScene(
    std::make_shared<FlowScene>(Item::NodeType::MainScene, QUuid::createUuid(), Item::DeploymentType::Main));
    for (const auto &scene : scenesMap_)
    {
        removeScene(scene.first);
    }
    selectedScene_ = nullptr;
}

std::vector<std::shared_ptr<FlowScene>> NodesScene::allScenes() const
{
    std::vector<std::shared_ptr<FlowScene>> scenes;
    for (const auto &scene : scenesMap_)
    {
        scenes.push_back(scene.second);
    }
    scenes.push_back(getMainScene());
    return scenes;
}

QJsonObject NodesScene::save() const
{
    QJsonObject retval {};
    QJsonArray arr {};

    arr.push_back(mainScene_->save());
    for (const auto &scene : scenesMap_)
    {
        arr.push_back(scene.second->save());
    }

    retval["scenes"] = arr;
    arr = {};

    return retval;
}

void NodesScene::restore(const QJsonObject &p)
{
    QJsonArray nodesJsonArray = p["scenes"].toArray();

    for (auto i : nodesJsonArray)
    {
        auto scene = std::make_shared<FlowScene>();
        scene->setFlowModel(Facade::Instance().model());
        if (static_cast<Item::NodeType>(i.toObject()["type"].toInt()) == Item::NodeType::MainScene)
        {
            setMainScene(scene);
            setTopView(new FlowView {});
            Facade::Instance().sceneMain();
        }
        else
        {
            auto id = i.toObject()["id"].toString();
            addScene(id, scene);
            Facade::Instance().scenePart(id, 2);
        }
    }

    for (auto i : nodesJsonArray)
    {
        FlowScene* scene {nullptr};
        if (static_cast<Item::NodeType>(i.toObject()["type"].toInt()) == Item::NodeType::MainScene)
        {
            scene = mainScene_.get();
        }
        else
        {
            scene = getScene(QUuid::fromString(i.toObject()["id"].toString())).get();
        }
        scene->restore(i.toObject());
    }
}

void NodesScene::restoreConnections(const QJsonObject &p)
{
    QJsonArray nodesJsonArray = p["scenes"].toArray();
    for (auto i : nodesJsonArray)
    {
        FlowScene* scene {nullptr};
        if (static_cast<Item::NodeType>(i.toObject()["type"].toInt()) == Item::NodeType::MainScene)
        {
            scene = mainScene_.get();
        }
        else
        {
            scene = getScene(QUuid::fromString(i.toObject()["id"].toString())).get();
        }
        scene->restoreConnections(i.toObject());
    }
}

void NodesScene::sceneSelectionChangedSlot()
{
    auto senderScene = qobject_cast<FlowScene*>(sender());

    if (nullptr == senderScene)
    {
        return;
    }
    if (senderScene == selectedScene_)
    {
        return;
    }
    if (senderScene->selectedItems().empty())
    {
        return;
    }
    selectedScene_ = senderScene;
}

QList<QGraphicsItem*> NodesScene::selectedItems() const
{
    QList<QGraphicsItem*> ret;
    if (nullptr != selectedScene_)
    {
        ret = selectedScene_->selectedItems();
    }
    return ret;
}

void NodesScene::clearSelection()
{
    if (nullptr != selectedScene_)
    {
        selectedScene_->clearSelection();
        selectedScene_ = nullptr;
    }
}
} // namespace QtNodes
