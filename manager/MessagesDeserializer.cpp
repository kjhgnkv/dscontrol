#include "MessagesDeserializer.hpp"

#include "nodeeditor/lists/Message.hpp"
#include "nodeeditor/lists/MessageList.hpp"
#include "models/MessageModel.hpp"
#include "nodeeditor/singletones/Facade.hpp"
#include <QDebug>
#include <QXmlStreamReader>

#define TAG_SENDER "Send"
#define TAG_RECEIVER "Recv"
#define TAG_PROTOCOL "Protocol"
#define TAG_INPROTOCOL "InternalProtocol"

namespace dscontrol
{
    bool MessagesDeserializer::getMessages1(const QDir& xmlDirectory)
    {
        qDebug() << "new start deserialize all messages from " + xmlDirectory.absolutePath();
        QList<quint64> listOfId;
        if (!xmlDirectory.isEmpty())
        {
            auto xmlFileNames = xmlDirectory.entryInfoList(QStringList() << "*.xml" << "*.XML",
                QDir::Readable | QDir::Files);
            QXmlStreamReader xmlReader;
            for (const auto& fileName : xmlFileNames)
            {
                QFile xmlFile(fileName.absoluteFilePath());

                if (xmlFile.open(QIODevice::ReadOnly))
                {
                    xmlReader.setDevice(&xmlFile);

                    while(xmlReader.readNextStartElement())
                    {
                        if (xmlReader.name() == "ModuleClass")
                        {
                            xmlReader.readNextStartElement();
                            break;
                        }
                    }
                    while (!xmlReader.atEnd())
                    {
                        if (xmlReader.name() == TAG_SENDER || xmlReader.name() == TAG_RECEIVER)
                        {
                            auto attributes = xmlReader.attributes();
                            QtNodes::Message newMessage{0};
                            bool isNewMessage = false;
                            for (const auto& attribute : attributes)
                            {
                                if (attribute.name() == "id")
                                {
                                    if (!listOfId.contains(attribute.value().toULongLong()))
                                    {
                                        newMessage.messId_ = attribute.value().toULongLong();
                                        listOfId.append(newMessage.messId_);
                                        isNewMessage = true;
                                    }
                                    else
                                    {
                                        isNewMessage = false;
                                        break;
                                    }
                                }
                                else if (attribute.name() == "name")
                                {
                                    newMessage.name_ = attribute.value().toString();
                                }
                                else if (attribute.name() == "protocol")
                                {
                                    newMessage.protocol_.name = attribute.value().toString();
                                }
                            }

                            if (!isNewMessage)
                            {
                                // xmlReader.isEndElement()
                                // ? xmlReader.readNextStartElement()
                                // : xmlReader.skipCurrentElement();

                                if (xmlReader.isEndElement())
                                {
                                    xmlReader.readNextStartElement();
                                }
                                else
                                {
                                    xmlReader.skipCurrentElement();
                                }
                                continue;
                            }
                            xmlReader.readNextStartElement();
                            for (auto tagName = xmlReader.name();
                                !xmlReader.atEnd();
                                xmlReader.readNextStartElement(),
                                tagName = xmlReader.name())
                            {
                                if (xmlReader.isEndElement())
                                {
                                    continue;
                                }
                                if (TAG_SENDER == tagName|| TAG_RECEIVER == tagName)
                                {
                                    break;
                                }
                                if (TAG_PROTOCOL != tagName && TAG_INPROTOCOL != tagName)
                                {
                                    continue;
                                }

                                QtNodes::Message::Protocol* proto;
                                proto = TAG_PROTOCOL == tagName ? &newMessage.protocol_ : &newMessage.internalProtocol_;

                                for (const auto& attribute : xmlReader.attributes())
                                {
                                    if (attribute.name() == "id")
                                    {
                                        proto->id = attribute.value().toInt();
                                    }
                                    else if (attribute.name() == "name")
                                    {
                                        proto->name = attribute.value().toString();
                                    }
                                    else if (attribute.name() == "factory")
                                    {
                                        proto->factory = attribute.value().toString();
                                    }
                                    else if (attribute.name() == "component")
                                    {
                                        proto->component = attribute.value().toString();
                                    }
                                }
                            }

                            auto msgList = QtNodes::Facade::Instance().messageList();
                            if (isNewMessage)
                            {
                                msgList->addMessage(std::make_shared<QtNodes::Message>(newMessage));
                            }
                        }
                        else
                        {
                            // xmlReader.isEndElement()
                            // ? xmlReader.readNextStartElement()
                            // : xmlReader.skipCurrentElement();

                            if (xmlReader.isEndElement())
                            {
                                xmlReader.readNextStartElement();
                            }
                            else
                            {
                                xmlReader.skipCurrentElement();
                            }
                        }
                       
                        if (xmlReader.hasError())
                        {
                            qCritical() << "Error:" << xmlFile.errorString();
                        }
                    } //while (!xmlReader.atEnd())
                    xmlFile.close();
                }
                else
                {
                    qDebug() << "Cannot open the file:" << fileName.fileName();
                }
            }
        }
        else
        {
            qWarning() << "Directory is empty:" << xmlDirectory.path();
            return false;
        }
        qDebug() << "finish deserialize all messages";
        return true;
    }
bool MessagesDeserializer::getMessages(const QDir &xmlDirectory)
{
    qDebug() << "start deserialize all messages from " + xmlDirectory.absolutePath();
    QList<quint64> listOfId;
    if (!xmlDirectory.isEmpty())
    {
        auto xmlFileNames = xmlDirectory.entryInfoList(QStringList() << "*.xml" << "*.XML",
                                                       QDir::Readable | QDir::Files);
        QXmlStreamReader xmlReader;
        for (const auto &fileName : xmlFileNames)
        {
            QFile xmlFile(fileName.absoluteFilePath());

            if (xmlFile.open(QIODevice::ReadOnly))
            {
                xmlReader.setDevice(&xmlFile);

                xmlReader.readNextStartElement();
                while (!xmlReader.atEnd())
                {
                    if (xmlReader.name() == "Send" || xmlReader.name() == "Recv")
                    {
                        auto attributes = xmlReader.attributes();
                        QtNodes::Message newMessage {0};
                        bool isNewMessage = false;
                        for (const auto &attribute : attributes)
                        {
                            if (attribute.name() == "id")
                            {
                                if (!listOfId.contains(attribute.value().toULongLong()))
                                {
                                    newMessage.messId_ = attribute.value().toULongLong();
                                    listOfId.append(newMessage.messId_);
                                    isNewMessage = true;
                                }
                                else
                                {
                                    isNewMessage = false;
                                    break;
                                }
                            }
                            else if (attribute.name() == "name")
                            {
                                newMessage.name_ = attribute.value().toString();
                            }
                            else if (attribute.name() == "protocol")
                            {
                                newMessage.protocol_.name = attribute.value().toString();
                            }
                        }

                        auto msgList = QtNodes::Facade::Instance().messageList();
                        QVector<int> roles;
                        if (isNewMessage)
                        {
                            if (auto id = QtNodes::Facade::Instance().messageList()->addMessage(); id >= 0 && msgList)
                            {
                                for (int column = MessageModel::Name; column < MessageModel::CommNames; ++column)
                                {
                                    QVariantList updated;
                                    switch (column)
                                    {
                                        case MessageModel::Name:
                                        {
                                            updated.append(newMessage.name_);
                                            break;
                                        }
                                        case MessageModel::Id:
                                        {
                                            updated.append(newMessage.messId_);
                                            break;
                                        }
                                        //TODO: remake to new structure
                                        case MessageModel::Protocol:
                                        {
                                            updated.append(newMessage.protocol_.name);
                                            break;
                                        }
                                    }
                                    msgList->updateMessage(id, updated, column);
                                }
                            }
                        }
                    }
                    else if (xmlReader.name() == "Protocol" || xmlReader.name() == "InternalProtocol")
                    {
                        qDebug() << "PROTOCOLS";
                    
                    }
                    if (xmlReader.hasError())
                    {
                        qCritical() << "Error:" << xmlFile.errorString();
                    }
                    xmlReader.readNextStartElement();
                }
                xmlFile.close();
            }
            else
            {
                qDebug() << "Cannot open the file:" << fileName.fileName();
            }
        }
    }
    else
    {
        qWarning() << "Directory is empty:" << xmlDirectory.path();
        return false;
    }
    qDebug() << "finish deserialize all messages";
    return true;
}
} // namespace dscontrol
