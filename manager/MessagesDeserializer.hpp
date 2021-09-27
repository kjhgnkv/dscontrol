#pragma once

#include <QDir>

namespace dscontrol
{
class MessageModel;

/*!
\brief Class performs xml-deserialization from xml directory
 */
class MessagesDeserializer
{
private:
    MessagesDeserializer() = default;
    ~MessagesDeserializer() = default;

public:
    /*!
    \brief Reads all messages from files in xml directory and fills Facade
     messages list
     */
    static bool getMessages1(const QDir &xmlDirectory);
    static bool getMessages(const QDir &xmlDirectory);
};
} // namespace dscontrol
