#include "NodeItemSniffer.hpp"

#include <QByteArray>
#include <QDataStream>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

NodeItemSniffer::NodeItemSniffer()
{
}

NodeItemSniffer::NodeItemSniffer(const QVariant &variant)
{
    setData(QModelIndex(), variant);
}

bool NodeItemSniffer::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole)
    {
        QJsonDocument jsonDocument = QJsonDocument::fromJson(value.toString().toUtf8());

        QJsonValue networkJsonValue = jsonDocument.object().value(QString("Network"));
        QJsonObject object = networkJsonValue.toObject();
        QJsonValue jsonValueSniffer = jsonDocument.object().value(QString("Sniffer"));
        //        qDebug() << "jsonValueSniffer" << jsonValueSniffer;
        //        if (jsonValueSniffer.isNull())
        //        {
        //            QJsonValue jsonValuePacket = jsonValueSniffer.toObject().value(QString("Packet"));
        //            qDebug() << "jsonValuePacket" << jsonValuePacket;
        //
        //            for (const auto& packet : jsonValuePacket.toArray())
        //            {
        //                QString sourceAddress = packet.toObject().value(QString("sourceAddress")).toString();
        //                QString destinationAddress = packet.toObject().value(QString("destinationAddress")).toString();
        //                QString protocol = packet.toObject().value(QString("protocol")).toString();
        //                QString length = packet.toObject().value(QString("length")).toString();
        //                QString data = packet.toObject().value(QString("data")).toString();
        //
        //                value_.append({object["hostName"].toString(), sourceAddress, destinationAddress, protocol, length, data});
        //            }
        //        }
    }

    return true;
}
