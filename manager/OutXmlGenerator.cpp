#include "OutXmlGenerator.hpp"

#include "SettingsManager.hpp"
#include <QDebug>
#include <QDir>
#include <QJsonArray>
#include <QJsonObject>
#include <algorithm>
#include <cmath>
#include <set>
#include "DSConfiguration.h"

inline void swap(QJsonValueRef v1, QJsonValueRef v2)
{
    QJsonValue temp(v1);
    v1 = QJsonValue(v2);
    v2 = temp;
}

namespace dscontrol
{
struct CommData
{
    QString commName;
    std::list<int> messageIds;
};

struct Comm
{
    QString method;
    QString name;
    QString send;
    QString recv;
    int fragment;
};

struct Mess
{
    QString name;
    QString protocol;
};

std::set<DeployObject> OutXmlGenerator::generateApplicationXmls(const QJsonObject &project)
{
    std::set<DeployObject> retval;

    auto model = project["model"].toObject();

    auto nodesArray = model["nodes"].toArray();
    QJsonArray applications;
    QJsonArray components;
    auto communications = project["communicationList"].toObject()["communications"].toArray();
    auto messages = project["messageList"].toObject()["messages"].toArray();

    for (const auto &node : nodesArray)
    {
        auto type = static_cast<QtNodes::Item::NodeType>(node.toObject()["type"].toInt());
        if (type == QtNodes::Item::NodeType::ApplicationNode)
        {
            applications.push_back(node.toObject());
        }
        else if (type == QtNodes::Item::NodeType::ComponentNode || type == QtNodes::Item::NodeType::ExternalNode)
        {
            components.push_back(node.toObject());
        }
    }

    std::sort(components.begin(), components.end(), [](const auto &val1, const auto &val2)
    {
        return val1.toObject()["name"].toString() < val2.toObject()["name"].toString();
    });

    for (const auto &applicationRef : applications)
    {
        auto application = applicationRef.toObject();

        std::vector<QDomElement> notLoaded;
        QDomDocument doc;

        auto instruction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
        doc.appendChild(instruction);
        auto root = doc.createElement("root");
        doc.appendChild(root);

        auto commandElement = doc.createElement("CommandType");
        auto commandValue = doc.createTextNode("Deploy");
        commandValue.setNodeValue("Deploy");
        root.appendChild(commandElement).appendChild(commandValue);

        auto applicationElement = doc.createElement("ApplicationName");
        auto applicationValue = doc.createTextNode("ApplicationName");
        applicationValue.setNodeValue(application["name"].toString());
        root.appendChild(applicationElement).appendChild(applicationValue);

        auto dsHostElement = doc.createElement("DSHost");
        auto dsHostValue = doc.createTextNode("DSHost");
        root.appendChild(dsHostElement);
        
        auto protoElement = doc.createElement("Protocols");
        auto commElement = doc.createElement("Communication");
        auto rootElement = doc.createElement("Components");
        QStringList protocolList{};
        QStringList commList{};


        for (const auto &componentRef : components)
        {
            auto component = componentRef.toObject();

            auto componentType = component["implementation"].toObject()["componentType"].toString();
            auto componentName = component["name"].toString();

            auto messagesIn = component["implementation"].toObject()["messagesIn"].toArray();
            auto messagesOut = component["implementation"].toObject()["messagesOut"].toArray();

            bool load {false};

            QDomElement componentNode;

            //TODO it is necessary? there can be several components with the same type
//             for (auto element = rootElement.firstChildElement(); !element.isNull(); element = element.nextSiblingElement())
//             {
//                 if (element.attribute("name") == componentType)
//                 {
//                     componentNode = element;
//                     break;
//                 }
//             }

            if (componentNode.isNull())
            {
                componentNode = doc.createElement("Component");
                componentNode.setAttribute("name", componentType);

                if (QUuid::fromString(component["parent"].toString()) == QUuid::fromString(application["id"].toString()))
                {
                    componentNode.setAttribute("load", "true");
                    load = true;
                }
                else
                {
                    componentNode.setAttribute("load", "false");
                    load = false;
                }
            }

            auto objectNode = doc.createElement("Object");
            objectNode.setAttribute("name", componentName);

            QJsonArray connections;
            if (auto arr = component["outConnections"].toArray(); !arr.isEmpty())
            {
                for (const auto &conn : arr)
                {
                    connections.push_back(conn);
                }
            }
            if (auto arr = component["inConnections"].toArray(); !arr.isEmpty())
            {
                for (const auto &conn : arr)
                {
                    connections.push_back(conn);
                }
            }

            if (connections.isEmpty())
            {
                break;
            }

            for (const auto &connection : connections)
            {
                if (connection.isNull())
                {
                    continue;
                }

                auto comm = doc.createElement("Comm");

                auto commName = connection.toObject()["communicationName"].toString();

                auto found = std::find_if(communications.begin(), communications.end(), [commName](const auto &val)
                {
                    if (val.toObject()["Name"].toString() == commName)
                    {
                        return true;
                    }

                    return false;
                });

                if (found == communications.end())
                {
                    continue;
                }

                auto communication = (*found).toObject();
                QString ipPort;
                QString method;
                QString methodComponent;
                bool fragment {false};
                bool in {false};

                if (QUuid::fromString(component["id"].toString()) == QUuid::fromString(connection.toObject()["out_id"].toString()))
                {
                    ipPort = communication["Send"].toString();
                   
                }
                else if (QUuid::fromString(component["id"].toString()) == QUuid::fromString(connection.toObject()["in_id"].toString()))
                {
                    in = true;
                    ipPort = communication["Recv"].toString();
                }
                method = communication["Method"].toString();
                methodComponent = communication["Component"].toString();
                fragment = communication["Fragment"].toBool();

                QString ipAttr = "addr";
                bool isServer = false;
                if(QUuid::fromString(component["id"].toString()) == QUuid::fromString(connection.toObject()["server_id"].toString()))
                {
                    commName += "_server";
                    ipAttr = "bind";
                    isServer = true;

                }

                comm.setAttribute("name", commName);
                //comm.setAttribute(in ? "if" : "of", ipPort);
                comm.setAttribute("factory", method);
                comm.setAttribute("component", methodComponent);
                comm.setAttribute("fragment", fragment ? "true" : "false");
                int messId = connection.toObject()["messageIdx"].toInt();
                comm.setAttribute("id", messId);
                //TODO: header data?
                comm.setAttribute("header", "true");
                comm.setAttribute("server", isServer ? "true" : "false");
                comm.setAttribute(ipAttr, ipPort);

                //objectNode.appendChild(comm);
                /*commElement.appendChild(comm);*/

                auto message = std::find_if(messages.begin(), messages.end(), [messId](const auto& val)
                {
                    if (val.toObject()["id"].toInt() == messId)
                    {
                        return true;
                    }
                    return false;
                });

                if (message != messages.end())
                {
                    auto protocol = (*message).toObject()["protocol"].toObject();
                    auto inprotocol = (*message).toObject()["inprotocol"].toObject();
                    auto proto = doc.createElement("Protocol");
                    auto inproto = doc.createElement("Protocol");
                    auto protoname = protocol["name"].toString();
                    auto inprotoname = inprotocol["name"].toString();
                    if (!protocolList.contains(protoname))
                    {
                        proto.setAttribute("name", protoname);
                        proto.setAttribute("id", protocol["id"].toInt());
                        proto.setAttribute("factory", protocol["factory"].toString());
                        proto.setAttribute("component", protocol["component"].toString());
                        protoElement.appendChild(proto);
                        protocolList.append(protoname);
                    }
                    if (!protocolList.contains(inprotoname))
                    {
                        inproto.setAttribute("name", inprotoname);
                        inproto.setAttribute("id", inprotocol["id"].toInt());
                        inproto.setAttribute("factory", inprotocol["factory"].toString());
                        inproto.setAttribute("component", inprotocol["component"].toString());
                        protoElement.appendChild(inproto);
                        protocolList.append(inprotoname);
                    }

                    comm.setAttribute("protocol", protoname);
                    comm.setAttribute("in_protocol", inprotoname);
                }
                if (!commList.contains(commName))
                {
                    commElement.appendChild(comm);
                    commList.append(commName);
                }

                if (!in)
                {
                    auto send = doc.createElement("Send");
                    auto toComponentId = QUuid::fromString(connection.toObject()["in_id"].toString());

                    auto found = std::find_if(components.begin(), components.end(), [toComponentId](const auto &val)
                    {
                        if (QUuid::fromString(val.toObject()["id"].toString()) == toComponentId)
                        {
                            return true;
                        }
                        return false;
                    });

                    if (found != components.end())
                    {
                        auto id = connection.toObject()["messageIdx"].toInt();
                        QString toComponent = (*found).toObject()["name"].toString();
                        send.setAttribute("destination", toComponent);
                        send.setAttribute("comm", commName);
                        send.setAttribute("id", id);

                        auto found = std::find_if(messagesOut.begin(), messagesOut.end(), [id](auto val)
                        {
                            if (val.toObject()["id"].toInt() == id)
                            {
                                return true;
                            }
                            return false;
                        });
                        if (found != messagesOut.end())
                        {
                            //send.setAttribute("protocol", (*found).toObject()["protocol"].toString());
                            send.setAttribute("name", (*found).toObject()["name"].toString());
                        }

                        objectNode.appendChild(send);
                    }
                }
                else
                {
                    auto recv = doc.createElement("Recv");
                    auto fromComponentId = QUuid::fromString(connection.toObject()["out_id"].toString());

                    auto found = std::find_if(components.begin(), components.end(), [fromComponentId](const auto &val)
                    {
                        if (QUuid::fromString(val.toObject()["id"].toString()) == fromComponentId)
                        {
                            return true;
                        }
                        return false;
                    });

                    if (found != components.end())
                    {
                        auto id = connection.toObject()["messageIdx"].toInt();
                        QString fromComponent = (*found).toObject()["name"].toString();
                        recv.setAttribute("source", fromComponent);
                        recv.setAttribute("comm", commName);
                        recv.setAttribute("id", id);

                        auto found_In = std::find_if(messagesIn.begin(), messagesIn.end(), [id](const auto &val)
                        {
                            if (val.toObject()["id"].toInt() == id)
                            {
                                return true;
                            }

                            return false;
                        });

                        if (found_In != messagesIn.end())
                        {
                            //recv.setAttribute("protocol", (*found_In).toObject()["protocol"].toString());
                            recv.setAttribute("name", (*found_In).toObject()["name"].toString());
                        }

                        objectNode.appendChild(recv);
                    }
                }
            }

            componentNode.appendChild(objectNode);

            dsHostElement.appendChild(protoElement);
            dsHostElement.appendChild(commElement);
            if (load)
            {
                rootElement.appendChild(componentNode);
                dsHostElement.appendChild(rootElement);
            }
            else
            {
                notLoaded.push_back(componentNode);
            }

            QFile out(application["name"].toString()+".xml");
            if (out.open(QIODevice::WriteOnly | QIODevice::Truncate))
            {
                out.write(doc.toByteArray());
                out.close();
            }
        }
        for (const auto &el : notLoaded)
        {
            dsHostValue.appendChild(rootElement.appendChild(el));
        }
        retval.insert({application["name"].toString(), doc});
    }

    return retval;
}

std::vector<std::tuple<int, QString, QString>> OutXmlGenerator::generateProjectHierarchy(const QJsonObject &project)
{
    std::vector<std::tuple<int, QString, QString>> retval;
    auto name = project["Name"].toString();
    retval.emplace_back(static_cast<int>(QtNodes::DSLevels::Project), name, "");

    auto uuid = project["Uuid"].toString();

    auto nodesArray = project[uuid].toObject()["model"].toObject()["nodes"].toArray();
    auto communications = project[uuid].toObject()["communicationList"].toObject()["communications"].toArray();
    for (const auto &node : nodesArray)
    {
        auto gen = generateLevelHierarchy(node.toObject(), communications);
        if (!gen.empty())
            std::move(gen.begin(), gen.end(), std::back_inserter(retval));
    }

    return retval;
}

std::set<UploadObject> OutXmlGenerator::generateUploadXmls(const QJsonObject &project)
{
    // TODO Deploy
    std::set<UploadObject> retval;

    auto model = project["model"].toObject();

    auto nodesArray = model["nodes"].toArray();
    QJsonArray components;
    QJsonArray applications;
    auto communications = project["communicationList"].toObject()["communications"].toArray();

    for (const auto &node : nodesArray)
    {
        auto type = static_cast<QtNodes::Item::NodeType>(node.toObject()["type"].toInt());

        if (type == QtNodes::Item::NodeType::ComponentNode || type == QtNodes::Item::NodeType::ExternalNode)
        {
            components.push_back(node.toObject());
        }

        if (type == QtNodes::Item::NodeType::ApplicationNode)
        {
            applications.push_back(node.toObject());
        }
    }

    std::sort(components.begin(), components.end(), [](const auto &val1, const auto &val2)
    {
        return val1.toObject()["name"].toString() < val2.toObject()["name"].toString();
    });

    // тут мы формируем xml для отправки
    for (const auto &componentRef : components)
    {
        auto component = componentRef.toObject();
        qDebug() << ">> component: " << component;
        auto type = component["implementation"].toObject()["componentType"].toString();
        auto path = component["implementation"].toObject()["componentFilePath"].toString();
        QString appName;

        QDomDocument doc;

        auto instruction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
        doc.appendChild(instruction);
        auto root = doc.createElement("root");
        doc.appendChild(root);

        auto commandElement = doc.createElement("CommandType");
        auto commandValue = doc.createTextNode("Upload");
        commandValue.setNodeValue("Upload");
        root.appendChild(commandElement).appendChild(commandValue);

        auto filePath = path;// getComponentFilePath(type, path);
        auto fileSize = getComponentSize(path);
        auto count = getComponentBlocksCount(fileSize);

        auto componentElement = doc.createElement("FileName");
        auto componentValue = doc.createTextNode("FileName");
        componentValue.setNodeValue(QFileInfo {filePath}.fileName());
        root.appendChild(componentElement).appendChild(componentValue);

        auto componentSizeElement = doc.createElement("FileSize");
        auto componentSizeValue = doc.createTextNode("FileSize");
        componentSizeValue.setNodeValue(QString::number(fileSize));
        root.appendChild(componentSizeElement).appendChild(componentSizeValue);

        auto componentCountElement = doc.createElement("BlocksCount");
        auto componentCountValue = doc.createTextNode("BlocksCount");
        componentCountValue.setNodeValue(QString::number(count));
        root.appendChild(componentCountElement).appendChild(componentCountValue);

        auto found = std::find_if(applications.begin(), applications.end(), [component](const QJsonValue &value) mutable
        {
            auto application = value.toObject();
            return QUuid::fromString(component["parent"].toString()) == QUuid::fromString(application["id"].toString());
        });

        if (found != applications.end())
        {
            appName = found->toObject()["name"].toString();
        }

        retval.insert({filePath, component["name"].toString(), appName, doc.toString()});
    }

    return retval;
}

std::set<ProgressDataObject> OutXmlGenerator::generateComputersMap(const QJsonObject &project)
{
    auto model = project["model"].toObject();
    std::set<ProgressDataObject> retval;
    std::list<QJsonObject> computers;
    std::list<QJsonObject> applications;
    std::list<QJsonObject> components;

    auto nodesArray = model["nodes"].toArray();
    auto communications = project["communicationList"].toObject()["communications"].toArray();

    for (const auto &node : nodesArray)
    {
        auto type = static_cast<QtNodes::Item::NodeType>(node.toObject()["type"].toInt());

        if (type == QtNodes::Item::NodeType::ComputerNode)
        {
            computers.push_back(node.toObject());
        }
        if (type == QtNodes::Item::NodeType::ApplicationNode)
        {
            applications.push_back(node.toObject());
        }
        if (type == QtNodes::Item::NodeType::ComponentNode)
        {
            components.push_back(node.toObject());
        }
    }

    for (const auto &computer : computers)
    {
        QString ipAddress;
        std::vector<unsigned int> sizes;
        std::map<QString, std::list<ProgressDataObject::Component>> apps;
        for (const auto &app : applications)
        {
            if (QUuid::fromString(app["parent"].toString()) == QUuid::fromString(computer["id"].toString()))
            {
                for (const auto &component : components)
                {
                    if (QUuid::fromString(component["parent"].toString()) == QUuid::fromString(app["id"].toString()))
                    {
                        auto filePath = getComponentFilePath(component["implementation"].toObject()["componentType"].toString(),
                            component["implementation"].toObject()["componentFilePath"].toString());
                        auto fileSize = getComponentSize(filePath);
                        auto count = getComponentBlocksCount(fileSize);
                        auto inConns = component["inConnections"].toArray();
                        auto outConns = component["outConnections"].toArray();

                        const auto &[it, ret] = apps.insert({
                                                            app["name"].toString(),
                                                            {{filePath, static_cast<unsigned int>(count)}}
                                                            });
                        if (!ret)
                        {
                            it->second.push_back({filePath, static_cast<unsigned int>(count)});
                        }

                        if (!inConns.isEmpty() && ipAddress.isEmpty())
                        {
                            auto commId = inConns.at(0)["in_index"].toInt();
                            auto found = std::find_if(communications.begin(),
                                                      communications.end(),
                                                      [commId](const auto &val)
                                                      {
                                                          if (val.toObject()["Id"] == commId)
                                                          {
                                                              return true;
                                                          }
                                                          return false;
                                                      });

                            if (found != communications.end())
                            {
                                ipAddress = (*found).toObject()["Recv"].toString().split(":").first();
                            }
                        }
                        else if (!outConns.isEmpty() && ipAddress.isEmpty())
                        {
                            auto commId = outConns.at(0)["in_index"].toInt();
                            auto found = std::find_if(communications.begin(),
                                                      communications.end(),
                                                      [commId](const auto &val)
                                                      {
                                                          if (val.toObject()["Id"] == commId)
                                                          {
                                                              return true;
                                                          }
                                                          return false;
                                                      });

                            if (found != communications.end())
                            {
                                ipAddress = (*found).toObject()["Send"].toString().split(":").first();
                            }
                        }
                    }
                }
            }
        }

        retval.insert(ProgressDataObject {computer["name"].toString(), apps, ipAddress});
    }

    return retval;
}

std::set<QByteArray> OutXmlGenerator::generateStopXmls(const QStringList &apps)
{
    std::set<QByteArray> retval;

    for (const auto &app : apps)
    {
        QDomDocument doc;

        auto instruction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
        doc.appendChild(instruction);
        auto root = doc.createElement("root");
        doc.appendChild(root);

        auto commandElement = doc.createElement("CommandType");
        auto commandValue = doc.createTextNode("Stop");
        commandValue.setNodeValue("Stop");
        root.appendChild(commandElement).appendChild(commandValue);

        auto nameElement = doc.createElement("ApplicationName");
        auto nameValue = doc.createTextNode(app);
        nameValue.setNodeValue(app);
        root.appendChild(nameElement).appendChild(nameValue);

        retval.insert(doc.toByteArray());
    }

    return retval;
}

QByteArray OutXmlGenerator::generateApplicationTemplateXmls(const QString &filePath)
{
    if (filePath.isEmpty())
    {
        return {};
    }

    QDomDocument doc;

    auto instruction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instruction);
    auto root = doc.createElement("root");
    doc.appendChild(root);

    auto commandElement = doc.createElement("CommandType");

    // TODO
    auto commandValue = doc.createTextNode("SendApplicationTemplate");
    commandValue.setNodeValue("SendApplicationTemplate");
    root.appendChild(commandElement).appendChild(commandValue);

    auto fileSize = getComponentSize(filePath);
    if (fileSize == 0)
    {
        qDebug() << "ERROR: fileSize == 0";
        return {};
    }
    auto count = getComponentBlocksCount(fileSize);

    auto componentElement = doc.createElement("FileName");
    auto componentValue = doc.createTextNode("FileName");
    componentValue.setNodeValue(QFileInfo {filePath}.fileName());
    root.appendChild(componentElement).appendChild(componentValue);

    auto componentSizeElement = doc.createElement("FileSize");
    auto componentSizeValue = doc.createTextNode("FileSize");
    componentSizeValue.setNodeValue(QString::number(fileSize));
    root.appendChild(componentSizeElement).appendChild(componentSizeValue);

    auto componentCountElement = doc.createElement("BlocksCount");
    auto componentCountValue = doc.createTextNode("BlocksCount");
    componentCountValue.setNodeValue(QString::number(count));
    root.appendChild(componentCountElement).appendChild(componentCountValue);

    return doc.toByteArray();
}

QByteArray OutXmlGenerator::generateGetServerData()
{
    QDomDocument doc;

    auto instruction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instruction);
    auto root = doc.createElement("root");
    doc.appendChild(root);

    auto commandElement = doc.createElement("CommandType");

    auto commandValue = doc.createTextNode("GetDSControlData");
    commandValue.setNodeValue("GetDSControlData");

    qDebug() << "GetDSControlData CommandType";

    root.appendChild(commandElement).appendChild(commandValue);

    return doc.toByteArray();
}

QByteArray OutXmlGenerator::generateSetServerData(const QString &data)
{
    QDomDocument doc;

    auto instruction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instruction);
    auto root = doc.createElement("root");
    doc.appendChild(root);

    auto commandElement = doc.createElement("CommandType");

    auto commandValue = doc.createTextNode("SetDSControlData");
    commandValue.setNodeValue("SetDSControlData");
    root.appendChild(commandElement).appendChild(commandValue);

    auto dataElement = doc.createElement("Data");
    auto datatValue = doc.createTextNode("Data");
    qDebug() << "SetDSControlData datatValue" << datatValue.data();
    datatValue.setNodeValue(data);
    root.appendChild(dataElement).appendChild(datatValue);

    return doc.toByteArray();
}

int dscontrol::OutXmlGenerator::getComponentSize(const QString &componentFilePath)
{
    QFileInfo file {componentFilePath};
    if (!file.exists())
        return 0;
    return file.size();
}

#define BUNDLE_PREFIX ".bndl"

QString dscontrol::OutXmlGenerator::getComponentFilePath(const QString &componentType, const QString &componentDir)
{
    return QString{componentDir + QDir::separator() + componentType + BUNDLE_PREFIX};
    //TODO: remove after testing;
    qDebug() << ">> componentType: " << componentType;
    const QString &name = componentType + BUNDLE_PREFIX;

    // мы используем не .dll и .so
    // вместо них испольузем бандлы, с расширением .bndl
    // бандлы могут хранить как Linux || Windows компоненты
    // надо сделать отдельную папку где будут хранится раздельно компоненты

    QDir folder {DSConfiguration::Instance().bundlesFolder()};
    if (!folder.exists())
    {
        qWarning() << "qWarning: DSConfiguration::Instance().bundlesFolder() << !folder.exists()";
        return {};
    }

    auto files = folder.entryInfoList();
    qDebug() << "files" << files;
    qDebug() << "name" << name;

    for (const auto &file : files)
    {
        if (file.fileName() == name)
        {
            return file.absoluteFilePath();
        }
    }

    qWarning() << "Bndl not found: " << name;
    return {};
}
} // namespace dscontrol

int dscontrol::OutXmlGenerator::getComponentBlocksCount(int componentSize)
{
    float blockSize = float(componentSize) / SettingsManager::blocksCountComponent;
    return std::ceil(blockSize);
}

std::vector<std::tuple<int, QString, QString>> dscontrol::OutXmlGenerator::generateLevelHierarchy(const QJsonObject &top, const QJsonArray &comms)
{
    std::vector<std::tuple<int, QString, QString>> retval;

    auto childs = top["childs"].toArray();
    auto type = static_cast<QtNodes::Item::NodeType>(top["type"].toInt());
    int level(0);

    switch (type)
    {
        case QtNodes::Item::NodeType::SystemNode:
        {
            level = static_cast<int>(QtNodes::DSLevels::System);
            break;
        }
        case QtNodes::Item::NodeType::SubSystemNode:
        {
            level = static_cast<int>(QtNodes::DSLevels::SubSystem);
            break;
        }
        case QtNodes::Item::NodeType::ComputerNode:
        {
            level = static_cast<int>(QtNodes::DSLevels::Computer);
            break;
        }
        case QtNodes::Item::NodeType::ApplicationNode:
        {
            level = static_cast<int>(QtNodes::DSLevels::Application);
            break;
        }
        case QtNodes::Item::NodeType::ComponentNode:
        {
            level = static_cast<int>(QtNodes::DSLevels::Component);
            break;
        }
    }

    for (const auto &child : childs)
    {
        auto tmp = child.toObject();
        auto ret = generateLevelHierarchy(child.toObject(), comms);
        std::move(ret.begin(), ret.end(), std::back_inserter(retval));
    }

    if (!retval.empty() || type == QtNodes::Item::NodeType::ComponentNode)
    {
        retval.insert(retval.begin(), std::make_tuple(level, top["name"].toString()
            , getNodeIP(top, comms)));
    }

    return retval;
}

QString dscontrol::OutXmlGenerator::getNodeIP(const QJsonObject& node, const QJsonArray& comms)
{
    auto outConn = node["outConnections"].toArray();
    for (const auto &conn : outConn)
    {
        auto commName = conn.toObject()["communicationName"].toString();
        qDebug() << "outConnections conn name: " << commName;
        auto found = std::find_if(comms.begin(),
            comms.end(),
            [commName](const auto& val)
        {
            if (val.toObject()["Name"].toString() == commName)
            {
                return true;
            }
            return false;
        });

        if (found != comms.end())
        {
            return (*found).toObject()["Send"].toString();
        }
    }

    auto inConn = node["inConnections"].toArray();
    for (const auto& conn : inConn)
    {
        auto commName = conn.toObject()["communicationName"].toString();
        qDebug() << "inConnections conn name: " << commName;
        auto found = std::find_if(comms.begin(),
            comms.end(),
            [commName](const auto& val)
        {
            if (val.toObject()["Name"].toString() == commName)
            {
                return true;
            }
            return false;
        });

        if (found != comms.end())
        {
            return (*found).toObject()["Recv"].toString();
        }
    }
    return {};
}

