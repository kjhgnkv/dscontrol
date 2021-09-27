#pragma once

#include <QString>
#include <QUuid>

#include "nodes/Serializable.hpp"
#include "common/nodeeditor_globals.hpp"

namespace QtNodes
{
#pragma pack(push, 1)
class Communication : public Serializable
{
public:
    explicit Communication(const int &id);
    ~Communication() = default;

    Device::Type deviceType_;
    QString deviceName_;
    QString commName_;
    int id_;
    QString connectionMethod_;
    QString methodComponent_;
    QString commSender_;
    QString commReceiver_;
    bool commFragment_;

    // Serializable interface
public:
    QJsonObject save() const;
    void restore(const QJsonObject &p);
};
#pragma pack(pop)
} // namespace QtNodes

