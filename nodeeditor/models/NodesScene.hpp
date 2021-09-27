#pragma once

#include "nodeeditor/nodeeditor_export.h"
#include "nodes/Serializable.hpp"

#include <QObject>
#include <QUuid>
#include <map>
#include <memory>
#include <vector>

class QGraphicsItem;

namespace QtNodes
{
class FlowScene;
class FlowView;

class UniversalGraphicsItemImpl;

class NODEEDITOR_EXPORT NodesScene : public QObject, public Serializable
{
Q_OBJECT
public:
    NodesScene();
    ~NodesScene() override;

    /**\brief adds scene of item with target uid
     */
    void addScene(const QUuid &uid, std::shared_ptr<FlowScene> scene);

    /**\brief removes scene of item with target uid
     */
    void removeScene(const QUuid &uid);

    /**\brief returns scene of item with target uid
     */
    std::shared_ptr<FlowScene> getScene(const QUuid &uid);

    /**\brief set the Main scene
     */
    void setMainScene(std::shared_ptr<FlowScene> scene);
    /**\brief returns the Main scene
     */
    std::shared_ptr<FlowScene> getMainScene() const;

    FlowScene* getTopScene() const;
    void setTopView(FlowView* view);
    FlowView* getTopView() const;

    /*!
    \return scene that has selected items
    */
    std::shared_ptr<FlowScene> getSelectedScene() const;

    void clear();

    std::vector<std::shared_ptr<FlowScene>> allScenes() const;

    QJsonObject save() const override;

    void restore(const QJsonObject &p) override;

    void restoreConnections(const QJsonObject &p);

    QList<QGraphicsItem*> selectedItems() const;

    void clearSelection();

private:
    void setTopScene(FlowScene* scene);

signals:
    void topSceneChanged(const FlowScene* oldScene);
    void topViewChanged(const FlowView* oldView);

private slots:

    void sceneSelectionChangedSlot();

private:
    std::map<QUuid, std::shared_ptr<FlowScene>> scenesMap_;

    std::shared_ptr<FlowScene> mainScene_;
    FlowScene* topScene_;
    FlowView* topView_;
    FlowScene* selectedScene_;
};
} // namespace QtNodes
