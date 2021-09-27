#pragma once

#include "DSHost/Communication/Service.h"
#include "DSHost/Communication/Sender.h"
#include "DSHost/Communication/Receiver.h"
#include "DSHost/Protocol/Service.h"
#include "DSHost/Transport/ChannelWrapper.h"
#include "DSHost/Protocol/ImplementationFactoryService.h"

using DS::communication::Sender;
using DS::communication::Receiver;
using DS::protocols::ProtocolService;
using DS::transport::TransportChannelWrapper;
using DS::protocols::ProtocolsImplementationFactoryService;

namespace DS {
namespace example {
namespace dsmonitor {
    class MessageHandler
    {
    public:
        typedef Poco::BasicEvent<const Receiver::ReceivedMessage> Event;

        explicit MessageHandler(
                Sender::Ptr pSender
                , Receiver::Ptr pReceiver
                , ProtocolsImplementationFactoryService::Ptr pImplementationFactoryService
                , ProtocolService::Ptr pProtoService);
        ~MessageHandler();

        Event packetAvailable;

        bool sendToDSServer(const std::string& msg);

    private:
        Sender::Ptr m_pSender;
        Receiver::Ptr m_pReceiver;
        ProtocolsImplementationFactoryService::Ptr m_pImplementationFactoryService;
        ProtocolService::Ptr m_pProtoService;

        void processMessage(const Receiver::ReceivedMessage& rd);
    };
}}}
