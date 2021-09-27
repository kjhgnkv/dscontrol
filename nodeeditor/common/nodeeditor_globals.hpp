// ItemsTypes.hpp

#pragma once

#include <QGraphicsItem>

namespace QtNodes
{

enum class DSLevels
{
    Project = 0,
    System,
    SubSystem,
    Computer,
    Application,
    Component
};

namespace Item
{
enum class NodeType
{
    None = QGraphicsItem::UserType + 1,
    ComponentNode,
    ApplicationNode,
    ComputerNode,
    SubSystemNode,
    SystemNode,
    ExternalNode,
    Frame,
    Note,
    Node,
    Port,
    Connection,
    Text,
    MainScene,
    ///< Please, insert new NodeType values in here instead of
    OtherType
    ///< ...this place
};

enum class PortType
{
    None,
    In,
    Out
};

enum class DeploymentType
{
    None,
    External,
    Internal,
    Main
};

enum class GraphicsItemProperties
{
    None,
    Lock,
    BorderColor,
    BorderWidth,
    BackColor,
    Color,
    Font,
    Picture,
    HideInnerLevels,
    ArrowDirection
};

} // namespace Item

namespace Arrows
{
enum class ArrowType
{
    NoType = QGraphicsItem::UserType + 30, ///< A default type value.
    Line,                                  ///< The straight line type.
    Curved,                                ///< The curved line type.
    PolyLine,                              ///< The polyline type.
    LateralVertical,                       ///< The lateral vertical type
    LateralHorizontal,                     ///< The lateral horizontal type
    TreeStyleHorizontal,
    TreeStyleVertical,
    Connection = QGraphicsItem::UserType + 40
};
}

namespace GeometricShapes
{
enum class ShapeType
{
    Annulus
};
}

namespace Device
{
enum class Type
{
    General = 0,
    Switch,
    Router,
    SerialPort,
    Cloud,
    ShareMemory,
    RadioDevice,
    None
};
// FIXME Think in future
const static QStringList types {
("General"), ("Switch"), ("Router"), ("Serial port"), ("Cloud"), ("Share memory"), ("Radio device"), ("None")
};
} // namespace Device

using CommId = int;
using MsgId = int;

const int Invalid = -1;

} // namespace QtNodes
