#pragma once

#include <QObject>
#include <QUuid>
#include <QVariantList>
#include <memory>

#include "nodes/Serializable.hpp"
#include "common/nodeeditor_globals.hpp"

namespace QtNodes
{
class Communication;

class CommunicationList final : public QObject, public Serializable
{
Q_OBJECT
public:
    //TODO: make it common with model!
    enum Columns
    {
        DeviceType,
        DeviceName,
        CommName,
        Id,
        Method,
        Component,
        Sender,
        Recv,
        Fragment,
        Size
    };

    explicit CommunicationList(QObject* parent = nullptr);
    ~CommunicationList() override;

    // Serializable interface
public:
    QJsonObject save() const override;
    void restore(const QJsonObject &p) override;

    size_t size();

    std::vector<std::shared_ptr<Communication>> communications();
    Device::Type communicationDeviceType(const int &commId);
    QString communicationDeviceName(const int &commId);
    QString communicationName(const int &commId);
    QString communicationConnectionMethod(const int &commId);
    QString communicationConnectionMethodComponent(const int& commId);
    QString communicationSender(const int &commId);
    QString communicationReceiver(const int &commId);
    bool communicationFragment(const int &commId);
    int communicationId(const int &row);

    int addCommunication();
    int addCommunication(std::shared_ptr<Communication>);
    bool removeCommunication(const int &commId);
    void updateCommunication(const int &commId, QVariantList updates, QVector<int> roles);

    void clear();

signals:
    void communicationAdded(int);
    void communicationToBeRemoved(int);
    void communicationToBeRemoved(QString);
    void communicationUpdated(int, QVector<int>);
    void updateError(const QString &error);

private:
    std::vector<std::shared_ptr<Communication>> communications_;
};

} // namespace QtNodes
