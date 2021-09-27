#pragma once

#include "DSHost/Communication/Service.h"
#include "DSHost/Communication/Sender.h"
#include "DSHost/Communication/Receiver.h"
#include "DSHost/Protocol/Service.h"
#include "DSHost/Transport/ChannelWrapper.h"
#include "DSHost/Event/Service.h"

using DS::communication::Sender;
using DS::communication::Receiver;
using DS::protocols::ProtocolService;
using DS::transport::TransportChannelWrapper;
using DS::events::EventService;

namespace DS {
namespace example {
    class MessageHandler
    {
    public:
        typedef Poco::BasicEvent<const Receiver::ReceivedData> Event;

        explicit MessageHandler(
                Sender::Ptr pSender
                , Receiver::Ptr pReceiver
                , ProtocolService::Ptr pProtoService);
        ~MessageHandler();

        Event packetAvailable;

        bool sendToDSServer(const std::vector<char>& msg, const std::string& ipAddress);

    private:
        Sender::Ptr m_pSender;
        Receiver::Ptr m_pReceiver;
        ProtocolService::Ptr m_pProtoService;

        void processPacket(const Receiver::ReceivedData& rd);
    };
}}
