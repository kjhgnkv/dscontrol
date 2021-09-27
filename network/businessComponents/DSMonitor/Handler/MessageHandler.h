#pragma once

#include "DSHost/Communication/Service.h"
#include "DSHost/Communication/Sender.h"
#include "DSHost/Communication/Receiver.h"
#include "DSHost/Protocol/Service.h"
#include "DSHost/Transport/ChannelWrapper.h"
#include "DSHost/Protocol/ImplementationFactoryService.h"
#include "DSHost/Transport/BroadcasterWithMQTT/ReceivedMsg.h"
#include "DSHost/Transport/MQTT/Defines.h"

#include <DSServerGlobal.h>

using DS::communication::Sender;
using DS::communication::Receiver;
using DS::protocols::ProtocolService;
using DS::transport::TransportChannelWrapper;
using DS::protocols::ProtocolsImplementationFactoryService;
using DS::transport::broadcaster_with_mqtt::ReceivedMsg;
using DS::dsserver::MessageReciverId;

namespace DS {
namespace dsmonitor {
    class MessageHandler
    {
    public:
        typedef Poco::BasicEvent<const ReceivedMsg::Ptr> Event;

        explicit MessageHandler(
                Sender::Ptr pSender
                , Receiver::Ptr pReceiver
                , ProtocolsImplementationFactoryService::Ptr pImplementationFactoryService
                , ProtocolService::Ptr pProtoService);
        ~MessageHandler();

        Event messageAvailable;

        bool sendToDSServer(const std::string& msg);

    private:
        Sender::Ptr m_pSender;
        Receiver::Ptr m_pReceiver;
        ProtocolsImplementationFactoryService::Ptr m_pImplementationFactoryService;
        ProtocolService::Ptr m_pProtoService;

        bool sendRawMessage(const char* msg
                            , const std::size_t msgLen
                            , const MessageReciverId::Enum reciverId
                            , const Sender::Dispatch::ParameterMap& pmap
                            , DS::transport::Error& outErrorId);

        void processMessage(const Receiver::ReceivedMessage& rd);
    };
}}
