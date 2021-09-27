#include "CommunicationList.hpp"

#include "Communication.hpp"
#include <QDebug>
#include <QJsonArray>
#include <QVector>
#include <QJsonDocument>
#include <QFile>

QtNodes::CommunicationList::CommunicationList(QObject* parent)
: QObject(parent)
{
    qDebug() << "CommunicationList: constructor";
}

QtNodes::CommunicationList::~CommunicationList()
{
    qDebug() << "CommunicationList: destructor";
}

QJsonObject QtNodes::CommunicationList::save() const
{
    qDebug() << "CommunicationList: save";
    QJsonObject retval;

    QJsonArray communications;
    for (const auto &comm : communications_)
    {
        communications.push_back(comm->save());
    }

    retval["communications"] = communications;

    qDebug() << communications;
    //TODO: for test remove
//     QFile file{"communications"};
//     if (file.open(QIODevice::WriteOnly))
//     {
//         file.write(QJsonDocument{retval}.toJson());
//         file.close();
//     }

    return retval;
}

void QtNodes::CommunicationList::restore(const QJsonObject &p)
{
    qDebug() << "CommunicationList: restore";
    QJsonArray arr = p["communications"].toArray();
    std::vector<std::shared_ptr<Communication>> vec {};

    for (const auto &comm : arr)
    {
        auto c = std::make_shared<Communication>(0);
        c->restore(comm.toObject());
        vec.push_back(c);
    }

    std::sort(vec.begin(), vec.end(), [](const auto &valS, const auto &valE)
    {
        return valS->id_ < valE->id_;
    });

    for (const auto &comm : vec)
    {
        addCommunication(comm);
    }
}

size_t QtNodes::CommunicationList::size()
{
    return communications_.size();
}

std::vector<std::shared_ptr<QtNodes::Communication>> QtNodes::CommunicationList::communications()
{
    return communications_;
}

QtNodes::Device::Type QtNodes::CommunicationList::communicationDeviceType(const int &commId)
{
    if (auto found = std::find_if(communications_.begin(), communications_.end(), [commId](const auto &val)
        {
            if (val->id_ == commId)
                return true;
            return false;
        });
    found != communications_.end())
    {
        return found->get()->deviceType_;
    }
    return {};
}

QString QtNodes::CommunicationList::communicationDeviceName(const int &commId)
{
    if (auto found = std::find_if(communications_.begin(), communications_.end(), [commId](const auto &val)
        {
            if (val->id_ == commId)
                return true;
            return false;
        });
    found != communications_.end())
    {
        return found->get()->deviceName_;
    }
    return {};
}

QString QtNodes::CommunicationList::communicationName(const int &commId)
{
    if (auto found = std::find_if(communications_.begin(), communications_.end(), [commId](const auto &val)
        {
            if (val->id_ == commId)
                return true;
            return false;
        });
    found != communications_.end())
    {
        return found->get()->commName_;
    }
    return {};
}

QString QtNodes::CommunicationList::communicationConnectionMethod(const int &commId)
{
    if (auto found = std::find_if(communications_.begin(), communications_.end(), [commId](const auto &val)
        {
            if (val->id_ == commId)
                return true;
            return false;
        });
    found != communications_.end())
    {
        return found->get()->connectionMethod_;
    }
    return {};
}

QString QtNodes::CommunicationList::communicationConnectionMethodComponent(const int& commId)
{
    if (auto found = std::find_if(communications_.begin(), communications_.end(), [commId](const auto& val)
    {
        if (val->id_ == commId)
            return true;
        return false;
    });
        found != communications_.end())
    {
        return found->get()->methodComponent_;
    }
    return {};
}

QString QtNodes::CommunicationList::communicationSender(const int &commId)
{
    if (auto found = std::find_if(communications_.begin(), communications_.end(), [commId](const auto &val)
        {
            if (val->id_ == commId)
                return true;
            return false;
        });
    found != communications_.end())
    {
        return found->get()->commSender_;
    }
    return {};
}

QString QtNodes::CommunicationList::communicationReceiver(const int &commId)
{
    if (auto found = std::find_if(communications_.begin(), communications_.end(), [commId](const auto &val)
        {
            if (val->id_ == commId)
                return true;
            return false;
        });
    found != communications_.end())
    {
        return found->get()->commReceiver_;
    }
    return {};
}

bool QtNodes::CommunicationList::communicationFragment(const int &commId)
{
    if (auto found = std::find_if(communications_.begin(), communications_.end(), [commId](const auto &val)
        {
            if (val->id_ == commId)
                return true;
            return false;
        });
    found != communications_.end())
    {
        return found->get()->commFragment_;
    }
    return false;
}

int QtNodes::CommunicationList::communicationId(const int &row)
{
    if (row < communications_.size())
    {
        return communications_.at(row)->id_;
    }
    return -1;
}

int QtNodes::CommunicationList::addCommunication()
{
    qDebug() << "addCommunication";
    //    QUuid uuid{};
    //    auto  found = std::find_if(communications_.begin(),
    //                              communications_.end(),
    //                              [comm](const auto &val) {
    //                                  if (comm == val.get())
    //                                  {
    //                                      return true;
    //                                  }
    //                                  return false;
    //                              });

    //    if (found != communications_.end())
    //    {
    //        uuid = QUuid::createUuid();
    auto comm = std::make_shared<QtNodes::Communication>(size());
    comm->commName_ = QString("Communication_%1").arg(comm->id_);
    communications_.push_back(comm);

    emit communicationAdded(int(size() - 1));

    return int(size() - 1);
}

int QtNodes::CommunicationList::addCommunication(std::shared_ptr<QtNodes::Communication> comm)
{
    qDebug() << "CommunicationList: add comm";
    communications_.push_back(comm);

    emit communicationAdded(comm->id_);

    return comm->id_;
}

bool QtNodes::CommunicationList::removeCommunication(const int &commId)
{
    qDebug() << "CommunicationList: remove comm";
    int row {};

    if (auto found = std::find_if(communications_.begin(), communications_.end(),
                                  [commId, &row](const auto &val) mutable
                                  {
                                      if (val->id_ == commId)
                                          return true;

                                      row++;
                                      return false;
                                  });
    found != communications_.end())
    {
        emit communicationToBeRemoved(row);
        emit communicationToBeRemoved(found->get()->commName_);

        auto next = communications_.erase(found);

        return true;
    }
    return false;
}

void QtNodes::CommunicationList::updateCommunication(const int &commId, QVariantList updates, QVector<int> roles)
{
    qDebug() << "CommunicationList: update comm";

    for (const auto &role : roles)
    {
        switch (role)
        {
            case Columns::DeviceType:
            {
                auto comm = communications_.at(commId);
                comm->deviceType_ = static_cast<Device::Type>(updates.takeFirst().toInt());
                if (comm->deviceType_ == Device::Type::None)
                {
                    comm->deviceName_ = tr("None");
                    if (!roles.contains(Columns::DeviceName))
                        roles.push_back(Columns::DeviceName);
                }
                break;
            }

            case Columns::DeviceName:
            {
                auto name = updates.takeFirst().toString();
                bool update = true;
                for (const auto &comm : communications_)
                {
                    if (comm->deviceName_ == name && comm != communications_.at(commId))
                    {
                        emit updateError(tr("Device name: '") + name + tr("' is existing!"));
                        update = false;
                        break;
                    }
                }
                if (update)
                {
                    if (!name.isEmpty())
                    {
                        communications_.at(commId)->deviceName_ = name;
                    }
                }
                break;
            }

            case Columns::CommName:
            {
                auto name = updates.takeFirst().toString();
                bool update = true;
                for (const auto &comm : communications_)
                {
                    if (comm->commName_ == name && comm != communications_.at(commId))
                    {
                        emit updateError(tr("Communication name: '") + name + tr("' is existing!"));
                        update = false;
                        break;
                    }
                }
                if (update)
                {
                    communications_.at(commId)->commName_ = name;
                }
                break;
            }

            case Columns::Method:
            {
                communications_.at(commId)->connectionMethod_ = updates.takeFirst().toString();
                break;
            }

            case Columns::Component:
            {
                communications_.at(commId)->methodComponent_ = updates.takeFirst().toString();
                break;
            }

            case Columns::Sender:
            {
                communications_.at(commId)->commSender_ = updates.takeFirst().toString();

                break;
            }

            case Columns::Recv:
            {
                communications_.at(commId)->commReceiver_ = updates.takeFirst().toString();

                break;
            }
            case Columns::Fragment:
            {
                communications_.at(commId)->commFragment_ = updates.takeFirst().toBool();

                break;
            }

            default:
                break;
        }
    }
    emit communicationUpdated(commId, roles);
}

void QtNodes::CommunicationList::clear()
{
    qDebug() << "CommunicationList: clear";
    for (const auto &comm : communications_)
    {
        emit communicationToBeRemoved(comm->id_);
    }
    communications_.clear();
}
