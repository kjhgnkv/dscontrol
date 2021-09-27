#pragma once

#include "nodeeditor/nodeeditor_export.h"
#include <memory>

class QMenu;
class QPointF;
class QGraphicsScene;

namespace QtNodes
{
class Connection;
class ToolBarItem;
class UniversalGraphicsItem;
class FlowScene;
namespace Item
{
enum class DeploymentType;
}
namespace Arrows
{
class AbstractArrow;
class ConnectionArrow;
} // namespace Arrows

class NODEEDITOR_EXPORT MenuBuilder
{
public:
    static QMenu* sceneMenu(FlowScene* scene, const QPointF &basePos);
    static QMenu* itemMenu(UniversalGraphicsItem* item);
    static QMenu* arrowMenu(QtNodes::Arrows::AbstractArrow* arrow);
    static QMenu* combineMenus(const QMenu* firstMenu, const QMenu* secondMenu = nullptr);
    static QMenu* connectionArrowMenu(QtNodes::Arrows::ConnectionArrow* connectionArrow);
    static QMenu* allConnectionsViewMenu(const std::shared_ptr<QtNodes::Connection> connection);
    static void setMenuEffect(QMenu* menu, bool toolBarFlag = false);
    static Item::DeploymentType createDeployTypeMenu();

private:
    static void insertMenuToMenu(QMenu* to, const QMenu* from);
    MenuBuilder() = delete;
    ~MenuBuilder() = delete;
};
}
