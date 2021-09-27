
#include "FlowItemMimeData.hpp"

#include <QJsonDocument>
#include <QJsonObject>

namespace QtNodes
{
FlowItemMimeData::FlowItemMimeData()
{
}

FlowItemMimeData::FlowItemMimeData(const QString &impName, const QJsonObject &info)
{
    impName_ = impName;
    setData(FlowItemMimeData::mimeType(), QJsonDocument {info}.toBinaryData());
}

QString FlowItemMimeData::mimeType()
{
    return "application/FlowItemMimeData";
}

const QString &FlowItemMimeData::impName() const
{
    return impName_;
}

QJsonObject FlowItemMimeData::info() const
{
    auto data = this->data(FlowItemMimeData::mimeType());
    auto jsonDoc = QJsonDocument::fromBinaryData(data);

    return jsonDoc.object();
}
} // namespace QtNodes
