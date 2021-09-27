#include "singletones/MenuBuilder.h"
#include "nodes/ComponentItemImpl.h"
#include "nodes/Connection.hpp"
#include "graphicsComposites/universalGraphicsObject/UniversalGraphicsItem.h"
#include "arrows/AbstractArrow.h"
#include "arrows/ConnectionArrow.h"
#include "graphicsComposites/universalGraphicsObject/imageSelector/ImageSelector.hpp"
#include "models/NodesScene.hpp"
#include "scenes/FlowScene.hpp"
#include "singletones/ClipboardController.hpp"
#include "singletones/Facade.hpp"
#include "graphicsComposites/text/TextGraphicsItem.hpp"
#include "graphicsComposites/universalGraphicsObject/toolbar/ToolBarWidget.h"
#include "views/FlowView.hpp"

#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QMenu>
#include <QMessageBox>
#include <QObject>

namespace QtNodes
{
QMenu* MenuBuilder::sceneMenu(FlowScene* scene, const QPointF &basePos)
{
    qDebug() << "MenuBuilder: sceneMenu";
    QMenu* sceneMenu = new QMenu();
    auto sceneType = scene->type();
    auto deploymentType = scene->deploymentType();
    if (sceneType != Item::NodeType::ComponentNode)
    {
        QAction* straightLineAction = sceneMenu->addAction(QIcon(":/contextMenuIcons/communication.png"),
                                                           QAction::tr("Straight Line"));
        QAction* curvedLineAction = sceneMenu->addAction(QIcon(":/contextMenuIcons/CurvedLine.png"),
                                                         QAction::tr("Curved Line"));
        QAction* customLineAction = sceneMenu->addAction(QIcon(":/contextMenuIcons/CustomLine.png"),
                                                         QAction::tr("Custom Line"));
        sceneMenu->addSeparator();

        auto isExternal = deploymentType == Item::DeploymentType::External;
        switch (sceneType)
        {
            case Item::NodeType::MainScene:
            {
                QAction* systemAction = sceneMenu->addAction(QIcon(":/contextMenuIcons/system.png"),
                                                             QAction::tr("System"));
                QObject::connect(systemAction, &QAction::triggered, [scene, deploymentType, basePos]() mutable
                {
                    deploymentType = createDeployTypeMenu();
                    if (deploymentType != Item::DeploymentType::None && deploymentType != Item::DeploymentType::Main)
                    {
                        auto idx = Facade::Instance().createNode(Item::NodeType::SystemNode, deploymentType, {}, {});
                        Facade::Instance().createComposite(idx, basePos, scene->id());
                    }
                });
                if (!isExternal)
                {
                    break;
                }
            }
                [[fallthrough]];
            case Item::NodeType::SystemNode:
            {
                QAction* subSystemAction = sceneMenu->addAction(QIcon(":/contextMenuIcons/subsystem.png"),
                                                                QAction::tr("Subsystem"));
                QObject::connect(subSystemAction, &QAction::triggered, [scene, deploymentType, basePos]()
                {
                    auto idx = Facade::Instance().createNode(Item::NodeType::SubSystemNode, deploymentType, {}, {});
                    Facade::Instance().createComposite(idx, basePos, scene->id());
                });
            }
                [[fallthrough]];
            case Item::NodeType::SubSystemNode:
            {
                QAction* computerAction = sceneMenu->addAction(QIcon(":/contextMenuIcons/computer.png"),
                                                               QAction::tr("Computer"));
                QObject::connect(computerAction, &QAction::triggered, [scene, deploymentType, basePos]()
                {
                    auto idx = Facade::Instance().createNode(Item::NodeType::ComputerNode, deploymentType, {}, {});
                    Facade::Instance().createComposite(idx, basePos, scene->id());
                });
                if (!isExternal)
                {
                    break;
                }
            }
                [[fallthrough]];
            case Item::NodeType::ComputerNode:
            {
                QAction* applicationAction = sceneMenu->addAction(QIcon(":/contextMenuIcons/csci.png"),
                                                                  QAction::tr("Application"));
                QObject::connect(applicationAction, &QAction::triggered, [scene, deploymentType, basePos]()
                {
                    auto idx = Facade::Instance().createNode(Item::NodeType::ApplicationNode, deploymentType, {}, {});
                    Facade::Instance().createComposite(idx, basePos, scene->id());
                });
                if (!isExternal)
                {
                    break;
                }
            }
                [[fallthrough]];
            default:
            {
                break;
            }
        }

        QAction* boundaryAction = sceneMenu->addAction(QIcon(":/contextMenuIcons/group.png"), QAction::tr("Boundary"));
        QAction* textElementAction = sceneMenu->addAction(QIcon(":/contextMenuIcons/text.png"),
                                                          QAction::tr("Text Element"));

        QObject::connect(straightLineAction, &QAction::triggered, [scene]()
        {
            scene->setState(FlowScene::State::AddArrow);
            scene->setNewArrowType(QtNodes::Arrows::ArrowType::Line);
        });
        QObject::connect(curvedLineAction, &QAction::triggered, [scene]()
        {
            scene->setState(FlowScene::State::AddArrow);
            scene->setNewArrowType(QtNodes::Arrows::ArrowType::Curved);
        });
        QObject::connect(customLineAction, &QAction::triggered, [scene]()
        {
            scene->setState(FlowScene::State::AddArrow);
            scene->setNewArrowType(QtNodes::Arrows::ArrowType::PolyLine);
        });
        QObject::connect(boundaryAction, &QAction::triggered, [scene, basePos]()
        {
            scene->addGraphicsFrame(basePos);
        });
        QObject::connect(textElementAction, &QAction::triggered, [scene, basePos]()
        {
            scene->addTextItem(basePos);
        });
    }

    const auto &controller = ClipboardController::Instance();
    sceneMenu->addActions({controller.copyAction(), controller.cutAction(), controller.pasteAction()});

    return sceneMenu;
}

QMenu* MenuBuilder::itemMenu(UniversalGraphicsItem* item)
{
    auto itemMenu = new QMenu;

    if (nullptr != item)
    {
        const auto itemType = static_cast<Item::NodeType>(item->type());

        if (itemType == Item::NodeType::ComponentNode)
        {
            QAction* setConnectionsAction = itemMenu->addAction(QAction::tr("Set Connections"));
            QObject::connect(setConnectionsAction, &QAction::triggered, [item]()
            {
                // FIXME change to soname funcction
                emit item->doubleClicked();
            });

            QAction* extractCodeAction = itemMenu->addAction(QString(QAction::tr("Extract code")));

            QObject::connect(extractCodeAction, &QAction::triggered, [item]()
            {
                auto model = Facade::Instance().model();
                auto id = item->id();

                auto index = model->nodeIndex(id);
                auto impl = model->nodeImplementation(index);
                auto cimpl = reinterpret_cast<QtNodes::ComponentItemImpl*>(impl);

                auto name = cimpl->componentType();
                Facade::Instance().extractCode(name);
            });
        }

        QAction* noteAction = itemMenu->addAction(QAction::tr("Note"));

        itemMenu->addSeparator();

        auto setImageAction = itemMenu->addAction(QIcon(":/contextMenuIcons/SetImage.png"), QAction::tr("Set image"));

        bool isLevelHidden = item->property(Item::GraphicsItemProperties::HideInnerLevels).toBool();

        // If node has component type, don't add hide/show inner level action
        if (itemType != Item::NodeType::ComponentNode)
        {
            const QString &actionWrd = isLevelHidden ? "Show" : "Hide";
            const QString &actionStr = QAction::tr(QString("%1 inner levels").arg(actionWrd).toStdString().c_str());
            QAction* hideLevelAction = itemMenu->addAction(actionStr);

            QObject::connect(hideLevelAction, &QAction::triggered, [item]()
            {
                const auto hideProperty = Item::GraphicsItemProperties::HideInnerLevels;
                item->setProperty(hideProperty, !item->property(hideProperty).toBool());
            });
        }

        auto lockUnlockAction = itemMenu->addAction(QIcon(":/contextMenuIcons/Lock.png"), QAction::tr("Lock"));
        if (item->isLocked())
        {
            lockUnlockAction->setIcon(QIcon(":/contextMenuIcons/Unlock.png"));
            lockUnlockAction->setText(QAction::tr("Unlock"));
        }
        auto renameAction = itemMenu->addAction(QAction::tr("Rename"));

        if (itemType == Item::NodeType::ApplicationNode)
        {
            if (!item->isGui())
            {
                itemMenu->addAction(QAction::tr("Toggle to GUI"), [item]
                {
                    item->setAsGui(true);
                });
            }
            else
            {
                itemMenu->addAction(QAction::tr("Toggle to console"), [item]
                {
                    item->setAsGui(false);
                });
            }
        }

        const auto &controller = ClipboardController::Instance();
        itemMenu->addActions({controller.copyAction(), controller.cutAction(), controller.pasteAction()});

        auto deleteObjectAction = itemMenu->addAction(QIcon(":/contextMenuIcons/delete.png"), QAction::tr("Delete"));

        QObject::connect(noteAction, &QAction::triggered, [item]()
        {
            if (auto scene = item->flowScene(); item)
            {
                // Old Note style
                /*scene->addNoteItem(
                    itemMenu->pos(),
                    {200, 100},
                    TextInputDialog::getText(QObject::tr("Note"),
                                             QObject::tr("Note")),
                    item);*/

                // New Note style (same as Text Item)
                auto note = scene->addTextBoxNote(item->scenePos() - QPointF {0, 150}, {200, 100}, item);

                note->setMinSize({200, 100});

                note->focusIn();
            }
        });

        QObject::connect(setImageAction, &QAction::triggered, [item]()
        {
            ImageSelector imageSelector(item->property(Item::GraphicsItemProperties::Picture).toString());
            imageSelector.exec();

            if (imageSelector.result() == QDialog::Rejected)
            {
                return;
            }
            item->setProperty(Item::GraphicsItemProperties::Picture, imageSelector.picturePath());
        });

        QObject::connect(lockUnlockAction, &QAction::triggered, [item]()
        {
            const auto lockProperty = Item::GraphicsItemProperties::Lock;
            item->setProperty(lockProperty, !item->property(lockProperty).toBool());
        });
        QObject::connect(renameAction, &QAction::triggered, [itemId = item->id()]()
        {
            Facade &facade = Facade::Instance();
            facade.renameNode(facade.model()->nodeIndex(itemId));
        });
        QObject::connect(deleteObjectAction, &QAction::triggered, [itemId = item->id()]()
        {
            Facade::Instance().removeComposite(itemId);
        });
    }

    return itemMenu;
}

QMenu* MenuBuilder::arrowMenu(Arrows::AbstractArrow* arrow)
{
    if (nullptr == arrow)
    {
        return nullptr;
    }

    const Arrows::ArrowType arrowType = arrow->arrowType();

    QMenu* arrowMenu = new QMenu();

    QMenu* lineStyleMenu = arrowMenu->addMenu(QMenu::tr("Line style"));
    QAction* straightLineAction = lineStyleMenu->addAction(QAction::tr("Straight line"));
    QAction* curvedLineAction = lineStyleMenu->addAction(QAction::tr("Curved line"));
    QAction* customLineAction = lineStyleMenu->addAction(QAction::tr("Custom line"));
    lineStyleMenu->addSeparator();
    QAction* lateralHorizontalLineAction = lineStyleMenu->addAction(QAction::tr("Lateral-Horizontal line"));
    QAction* lateralVerticalLineAction = lineStyleMenu->addAction(QAction::tr("Lateral-Vertical line"));
    lineStyleMenu->addSeparator();
    QAction* treeStyleHorizontalLineAction = lineStyleMenu->addAction(QAction::tr("Tree Style-Horizontal line"));
    QAction* treeStyleVerticalLineAction = lineStyleMenu->addAction(QAction::tr("Tree Style-Vertical line"));

    // Add shadow effect
    lineStyleMenu->setFixedWidth(250);
    setMenuEffect(lineStyleMenu);

    QMenu* changeDirectionMenu = arrowMenu->addMenu(QMenu::tr("Change direction"));

    QAction* directDirectionAction = changeDirectionMenu->addAction(QAction::tr("Direct"));
    QAction* reverseDirectionAction = changeDirectionMenu->addAction(QAction::tr("Reverse"));
    QAction* bothDirectionAction = changeDirectionMenu->addAction(QAction::tr("Bi-directional"));
    QAction* noDirectionAction = changeDirectionMenu->addAction(QAction::tr("No direction"));

    // Add shadow effect
    changeDirectionMenu->setFixedWidth(200);
    setMenuEffect(changeDirectionMenu);

    QIcon checkIcon(":/contextMenuIcons/CheckBox.png");
    if (Arrows::ArrowType::Line == arrowType || Arrows::ArrowType::PolyLine == arrowType)
    {
        QMenu* pinEndsMenu = arrowMenu->addMenu(QMenu::tr("Pin End(s)"));

        QAction* pinStartPointAction = pinEndsMenu->addAction(QAction::tr("Pin Start Point"));
        QAction* pinEndPointAction = pinEndsMenu->addAction(QAction::tr("Pin End Point"));
        QAction* pinBothEndsAction = pinEndsMenu->addAction(QAction::tr("Pin Both Ends"));
        QAction* unpinBothEndsAction = pinEndsMenu->addAction(QAction::tr("Unpin Both Ends"));

        // Add shadow effect
        pinEndsMenu->setFixedWidth(200);
        setMenuEffect(pinEndsMenu);

        pinEndsMenu->setObjectName("pinEndsMenu");
        pinStartPointAction->setObjectName("pinStartPointAction");
        pinEndPointAction->setObjectName("pinEndPointAction");
        pinBothEndsAction->setObjectName("pinBothEndsAction");
        unpinBothEndsAction->setObjectName("unpinBothEndsAction");

        bool isStartPointPinned = !arrow->isStartPointMovable();
        bool isEndPointPinned = !arrow->isEndPointMovable();
        pinBothEndsAction->setDisabled(isStartPointPinned && isEndPointPinned);
        unpinBothEndsAction->setEnabled(isStartPointPinned || isEndPointPinned);

        if (isStartPointPinned)
        {
            pinStartPointAction->setIcon(checkIcon);
        }
        if (isEndPointPinned)
        {
            pinEndPointAction->setIcon(checkIcon);
        }

        QObject::connect(pinStartPointAction, &QAction::triggered, [arrow]
        {
            arrow->setStartPointMovable(false);
        });

        QObject::connect(pinEndPointAction, &QAction::triggered, [arrow]
        {
            arrow->setEndPointMovable(false);
        });

        QObject::connect(pinBothEndsAction, &QAction::triggered, [arrow]
        {
            arrow->setStartPointMovable(false);
            arrow->setEndPointMovable(false);
        });

        QObject::connect(unpinBothEndsAction, &QAction::triggered, [arrow]
        {
            arrow->setStartPointMovable(true);
            arrow->setEndPointMovable(true);
            emit arrow->needToUpdateLastArrowPosition();
        });
    }

    QAction* deleteArrowAction = arrowMenu->addAction(QIcon(":/contextMenuIcons/delete.png"), QAction::tr("Delete"));

    arrowMenu->setObjectName("arrowMenu");
    lineStyleMenu->setObjectName("lineStyleMenu");
    straightLineAction->setObjectName("straightLineAction");
    curvedLineAction->setObjectName("curvedLineAction");
    customLineAction->setObjectName("customLineAction");
    lateralHorizontalLineAction->setObjectName("lateralHorizontalLineAction");
    lateralVerticalLineAction->setObjectName("lateralVerticalLineAction");
    treeStyleHorizontalLineAction->setObjectName("treeStyleHorizontalLineAction");
    treeStyleVerticalLineAction->setObjectName("treeStyleVerticalLineAction");

    changeDirectionMenu->setObjectName("changeDirectionMenu");
    directDirectionAction->setObjectName("directDirectionAction");
    reverseDirectionAction->setObjectName("reverseDirectionAction");
    bothDirectionAction->setObjectName("bothDirectionAction");
    noDirectionAction->setObjectName("noDirectionAction");
    deleteArrowAction->setObjectName("deleteArrowAction");

    switch (arrow->direction())
    {
        case Arrows::Direction::Direct:
        {
            directDirectionAction->setIcon(checkIcon);
            break;
        }
        case Arrows::Direction::Reverse:
        {
            reverseDirectionAction->setIcon(checkIcon);
            break;
        }
        case Arrows::Direction::Both:
        {
            bothDirectionAction->setIcon(checkIcon);
            break;
        }
        case Arrows::Direction::NoDirection:
        {
            noDirectionAction->setIcon(checkIcon);
            break;
        }
    }

    switch (arrowType)
    {
        case Arrows::ArrowType::Line:
        {
            straightLineAction->setIcon(checkIcon);
            break;
        }
        case Arrows::ArrowType::Curved:
        {
            curvedLineAction->setIcon(checkIcon);
            break;
        }
        case Arrows::ArrowType::PolyLine:
        {
            customLineAction->setIcon(checkIcon);
            break;
        }
        case Arrows::ArrowType::LateralHorizontal:
        {
            lateralHorizontalLineAction->setIcon(checkIcon);
            break;
        }
        case Arrows::ArrowType::LateralVertical:
        {
            lateralVerticalLineAction->setIcon(checkIcon);
            break;
        }
        case Arrows::ArrowType::TreeStyleHorizontal:
        {
            treeStyleHorizontalLineAction->setIcon(checkIcon);
            break;
        }
        case Arrows::ArrowType::TreeStyleVertical:
        {
            treeStyleVerticalLineAction->setIcon(checkIcon);
            break;
        }
        default:
        {
            break;
        }
    }

    QObject::connect(straightLineAction, &QAction::triggered, [arrow]()
    {
        if (Arrows::ArrowType::Line != arrow->arrowType())
        {
            emit arrow->arrowChangingType(Arrows::ArrowType::Line);
        }
    });

    QObject::connect(curvedLineAction, &QAction::triggered, [arrow]()
    {
        if (Arrows::ArrowType::Curved != arrow->arrowType())
        {
            emit arrow->arrowChangingType(Arrows::ArrowType::Curved);
        }
    });

    QObject::connect(customLineAction, &QAction::triggered, [arrow]()
    {
        if (Arrows::ArrowType::PolyLine != arrow->arrowType())
        {
            emit arrow->arrowChangingType(Arrows::ArrowType::PolyLine);
        }
    });

    QObject::connect(lateralHorizontalLineAction, &QAction::triggered, [arrow]()
    {
        if (Arrows::ArrowType::LateralHorizontal != arrow->arrowType())
        {
            emit arrow->arrowChangingType(Arrows::ArrowType::LateralHorizontal);
        }
    });

    QObject::connect(lateralVerticalLineAction, &QAction::triggered, [arrow]()
    {
        if (Arrows::ArrowType::LateralVertical != arrow->arrowType())
        {
            emit arrow->arrowChangingType(Arrows::ArrowType::LateralVertical);
        }
    });

    QObject::connect(treeStyleHorizontalLineAction, &QAction::triggered, [arrow]()
    {
        if (Arrows::ArrowType::TreeStyleHorizontal != arrow->arrowType())
        {
            emit arrow->arrowChangingType(Arrows::ArrowType::TreeStyleHorizontal);
        }
    });

    QObject::connect(treeStyleVerticalLineAction, &QAction::triggered, [arrow]()
    {
        if (Arrows::ArrowType::TreeStyleVertical != arrow->arrowType())
        {
            emit arrow->arrowChangingType(Arrows::ArrowType::TreeStyleVertical);
        }
    });

    QObject::connect(directDirectionAction, &QAction::triggered, [arrow]()
    {
        if (Arrows::Direction::Direct != arrow->direction())
        {
            arrow->setProperty(Item::GraphicsItemProperties::ArrowDirection,
                               QVariant {static_cast<int>(Arrows::Direction::Direct)});
        }
    });

    QObject::connect(reverseDirectionAction, &QAction::triggered, [arrow]()
    {
        if (Arrows::Direction::Reverse != arrow->direction())
        {
            arrow->setProperty(Item::GraphicsItemProperties::ArrowDirection,
                               QVariant {static_cast<int>(Arrows::Direction::Reverse)});
        }
    });

    QObject::connect(bothDirectionAction, &QAction::triggered, [arrow]()
    {
        if (Arrows::Direction::Both != arrow->direction())
        {
            QVariant value(static_cast<int>(Arrows::Direction::Both));
            arrow->setProperty(Item::GraphicsItemProperties::ArrowDirection, value);
        }
    });

    QObject::connect(noDirectionAction, &QAction::triggered, [arrow]()
    {
        if (Arrows::Direction::NoDirection != arrow->direction())
        {
            arrow->setProperty(Item::GraphicsItemProperties::ArrowDirection,
                               QVariant {static_cast<int>(Arrows::Direction::NoDirection)});
        }
    });

    QObject::connect(deleteArrowAction, &QAction::triggered, arrow, &Arrows::AbstractArrow::destroyMe);

    return arrowMenu;
}

QMenu* MenuBuilder::combineMenus(const QMenu* firstMenu, const QMenu* secondMenu)
{
    qDebug() << "MenuBuilder: combineMenus";
    QMenu* retValue = nullptr;
    if (nullptr != firstMenu)
    {
        retValue = new QMenu;
        for (QAction* action : firstMenu->actions())
        {
            if (nullptr != action)
            {
                if (action->isSeparator())
                {
                    retValue->addSeparator();
                }
                else if (nullptr != action->menu())
                {
                    QMenu* newRetValue = combineMenus(retValue, action->menu());
                    delete retValue;
                    retValue = newRetValue;
                }
                else
                {
                    retValue->addAction(action);
                }
            }
        }
        if (nullptr != secondMenu)
        {
            insertMenuToMenu(retValue, secondMenu);
        }
    }
    return retValue;
}

QMenu* MenuBuilder::connectionArrowMenu(Arrows::ConnectionArrow* connectionArrow)
{
    qDebug() << "MenuBuilder: connectionArrowMenu";
    QMenu* connectionArrowMenu = nullptr;
    if (nullptr != connectionArrow)
    {
        connectionArrowMenu = new QMenu;
        QAction* hideConnectionAction = connectionArrowMenu->addAction(QAction::tr("Hide"));
        QAction* deleteConnectionAction = connectionArrowMenu->addAction(QIcon(":/contextMenuIcons/delete.png"),
                                                                         QAction::tr("Delete"));

        QObject::connect(hideConnectionAction, &QAction::triggered, [=]()
        {
            connectionArrow->hide();
            auto sceneModel = Facade::Instance().sceneModel();
            if (nullptr != sceneModel)
            {
                auto allScenes = sceneModel->allScenes();
                for (auto scene : allScenes)
                {
                    if (nullptr != scene)
                    {
                        scene->hideConnections(connectionArrow->connections());
                    }
                }
            }
        });

        QObject::connect(deleteConnectionAction, &QAction::triggered, [=]
        {
            Facade::Instance().removeConnections(connectionArrow->connections());
        });

        hideConnectionAction->setObjectName("hideConnectionAction");
        deleteConnectionAction->setObjectName("deleteConnectionAction");
    }
    return connectionArrowMenu;
}

QMenu* MenuBuilder::allConnectionsViewMenu(const std::shared_ptr<Connection> connection)
{
    qDebug() << "MenuBuilder: allConnectionsViewMenu";
    QMenu* setConnectionVisibleMenu = new QMenu;
    QAction* hideConnectionAction = setConnectionVisibleMenu->addAction(QAction::tr("Hide"));
    QAction* showConnectionAction = setConnectionVisibleMenu->addAction(QAction::tr("Show"));

    QObject::connect(hideConnectionAction, &QAction::triggered, [=]()
    {
        auto sceneModel = Facade::Instance().sceneModel();
        if (nullptr != sceneModel)
        {
            auto allScenes = sceneModel->allScenes();
            for (auto scene : allScenes)
            {
                if (nullptr != scene)
                {
                    scene->hideConnections({connection.get()});
                }
            }
        }
    });

    QObject::connect(showConnectionAction, &QAction::triggered, [=]()
    {
        auto sceneModel = Facade::Instance().sceneModel();
        if (nullptr != sceneModel)
        {
            auto allScenes = sceneModel->allScenes();
            for (auto scene : allScenes)
            {
                if (nullptr != scene)
                {
                    scene->showConnections({connection.get()});
                }
            }
        }
    });

    setConnectionVisibleMenu->setObjectName("setConnectionVisibleMenu");
    hideConnectionAction->setObjectName("hideConnectionAction");
    showConnectionAction->setObjectName("showConnectionAction");
    return setConnectionVisibleMenu;
}

void MenuBuilder::setMenuEffect(QMenu* menu, bool toolBarFlag)
{
    // Set menu frame properties
    menu->setWindowFlag(Qt::FramelessWindowHint);
    menu->setWindowFlag(Qt::NoDropShadowWindowHint);
    menu->setAttribute(Qt::WA_TranslucentBackground);

    // Set shadow
    auto effect = new QGraphicsDropShadowEffect(menu);
    effect->setColor("#808080");
    effect->setOffset(0, 0);
    effect->setBlurRadius(10);
    menu->setGraphicsEffect(effect);

    // If toolbar is needed, update top margin
    const quint8 marginValue = toolBarFlag ? 40 : 10; ///< Maybe, this is gonna maaagiiic!
    const QString styleSheetStr = QString(R"(QMenu {margin-top: %1px;})").arg(marginValue);
    menu->setStyleSheet(menu->styleSheet() + styleSheetStr);
}

Item::DeploymentType MenuBuilder::createDeployTypeMenu()
{
    auto menu = new QMenu {};
    Item::DeploymentType retType = Item::DeploymentType::None;

    auto internal = menu->addAction(QAction::tr("Internal"));
    QObject::connect(internal, &QAction::triggered, [&retType]() mutable
    {
        retType = Item::DeploymentType::Internal;
    });

    auto external = menu->addAction(QAction::tr("External"));
    QObject::connect(external, &QAction::triggered, [&retType]() mutable
    {
        retType = Item::DeploymentType::External;
    });

    MenuBuilder::setMenuEffect(menu);
    menu->exec(QCursor::pos());
    menu->deleteLater();
    return retType;
}

void MenuBuilder::insertMenuToMenu(QMenu* to, const QMenu* from)
{
    qDebug() << "MenuBuilder: insertMenuToMenu";
    if (nullptr != to && nullptr != from)
    {
        for (QAction* action : from->actions())
        {
            if (nullptr != action)
            {
                if (action->isSeparator())
                {
                    to->addSeparator();
                }
                else if (nullptr != action->menu())
                {
                    insertMenuToMenu(to, action->menu());
                }
                else
                {
                    to->addAction(action);
                }
            }
        }
    }
}
} // namespace QtNodes
