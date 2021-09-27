#include "MimeGenerator.hpp"

#include "nodes/Node.hpp"
#include "nodes/NodeIndex.hpp"
#include "graphicsComposites/universalGraphicsObject/UniversalSelectableItem.h"
#include "lists/MessageList.hpp"
#include "NodesScene.hpp"
#include "nodes/FlowItemMimeData.hpp"
#include "singletones/Facade.hpp"

#include <QDebug>
#include <QDrag>
#include <QJsonArray>
#include <QJsonObject>

namespace QtNodes
{
void MimeGenerator::generateSystemDragMime(QObject* parent)
{
    qDebug() << "generate system mime";
    QJsonObject name;
    name["caption"] = "System";
    QJsonObject node;
    node["implementation"] = name;

    QMimeData* deviceMime = new QtNodes::FlowItemMimeData {"SystemNode", node};

    QDrag* drag = new QDrag {parent};
    drag->setMimeData(deviceMime);

    drag->exec(Qt::CopyAction, Qt::CopyAction);
}

void MimeGenerator::generateSubsystemDragMime(QObject* parent)
{
    qDebug() << "generate subsystem mime";
    QJsonObject name;
    name["caption"] = "Subsystem";
    QJsonObject node;
    node["implementation"] = name;

    QMimeData* deviceMime = new QtNodes::FlowItemMimeData {"SubsystemNode", node};

    QDrag* drag = new QDrag {parent};
    drag->setMimeData(deviceMime);

    drag->exec(Qt::CopyAction, Qt::CopyAction);
}

void MimeGenerator::generateComputerDragMime(QObject* parent)
{
    qDebug() << "generate device mime";
    QJsonObject name;
    name["caption"] = "Computer";
    QJsonObject node;
    node["implementation"] = name;

    QMimeData* deviceMime = new QtNodes::FlowItemMimeData {"DeviceNode", node};

    QDrag* drag = new QDrag {parent};
    drag->setMimeData(deviceMime);

    drag->exec(Qt::CopyAction, Qt::CopyAction);
}

void MimeGenerator::generateApplicationDragMime(QObject* parent)
{
    qDebug() << "generate application mime";
    QJsonObject name;
    name["caption"] = "Application";
    QJsonObject node;
    node["implementation"] = name;

    QMimeData* deviceMime = new QtNodes::FlowItemMimeData {"ApplicationNode", node};

    QDrag* drag = new QDrag {parent};
    drag->setMimeData(deviceMime);

    drag->exec(Qt::CopyAction, Qt::CopyAction);
}

void MimeGenerator::generateExternalDragMime(QObject* parent)
{
    // FIXME rewrite to json array
    qDebug() << "generate external mime";
    QJsonObject impl;
    QJsonObject node;
    impl["name"] = "External component";
    impl["caption"] = impl["name"].toString();

    impl["messagesIn"] = QtNodes::Facade::Instance().messageList()->save()["messages"].toArray();
    impl["messagesOut"] = impl["messagesIn"].toArray();

    node["implementation"] = impl;

    QMimeData* deviceMime = new QtNodes::FlowItemMimeData {"ExternalNode", node};

    QDrag* drag = new QDrag {parent};
    drag->setMimeData(deviceMime);

    drag->exec(Qt::CopyAction, Qt::CopyAction);
}

void MimeGenerator::generateTextDragMime(QObject* parent)
{
    qDebug() << "generate text mime";
    QJsonObject name;
    name["caption"] = "Text";
    QJsonObject node;
    node["implementation"] = name;

    QMimeData* deviceMime = new QtNodes::FlowItemMimeData {"TextNode", node};

    QDrag* drag = new QDrag {parent};
    drag->setMimeData(deviceMime);

    drag->exec(Qt::CopyAction, Qt::CopyAction);
}

void MimeGenerator::generateFrameDragMime(QObject* parent)
{
    qDebug() << "generate frame mime";
    QJsonObject name;
    name["caption"] = "Frame";
    QJsonObject node;
    node["implementation"] = name;

    QMimeData* deviceMime = new QtNodes::FlowItemMimeData {"FrameNode", node};

    QDrag* drag = new QDrag {parent};
    drag->setMimeData(deviceMime);

    drag->exec(Qt::CopyAction, Qt::CopyAction);
}

QMimeData* MimeGenerator::generateCopyMime(QList<QGraphicsItem*> objects)
{
    qDebug() << "generate copy mime";
    QJsonObject node;
    node["name"] = "Copy";

    QRectF itemsRect {};

    QJsonArray nodes {};

    for (const auto &object : objects)
    {
        QJsonObject value {};
        auto item = dynamic_cast<UniversalSelectableItem*>(object);
        if (item)
        {
            auto index = Facade::Instance().model()->nodeIndex(item->id());
            if (index.isValid())
            {
                value["model"] = index.nodePointer()->unsafeSave();
                nodes.push_back(value);
            }
            else
            {
                value["view"] = item->save();
                nodes.push_back(value);
            }
        }

        itemsRect |= object->sceneBoundingRect();
    }

    node["nodes"] = nodes;
    node["rect"] = QJsonObject {{  "x"     , itemsRect.x()}
                                , {"y"     , itemsRect.y()}
                                , {"width" , itemsRect.width()}
                                , {"height", itemsRect.height()}
    };

    QMimeData* retval = new QtNodes::FlowItemMimeData {"Copy", node};

    return retval;
}
} // namespace QtNodes
