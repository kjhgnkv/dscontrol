#pragma once

#include <scopedPtr.h>
#include <Poco/RWLock.h>
#include <queue>
#include <blocker.h>
#include "NetworkAccessory.h"

#include "MessageHandler.h"

namespace DS {
namespace example {
    class CommandHandler
    {
    public:
        explicit CommandHandler(Sender::Ptr pSender
                                , Receiver::Ptr pReceiver
                                , ProtocolService::Ptr pProtoService);
        ~CommandHandler();

        bool init();
        bool exec();

    private:
        static const long msc_waitMS;


        ds::utils::ScopedPtr<MessageHandler> m_messageHandler;
        std::queue<Poco::SharedPtr<IncomingMsg> > m_lastMsgs;
        std::set<std::string> m_ipAddress;

        ds::utils::Blocker m_blocker;

        Poco::RWLock m_lastMsgsLock;
        Poco::RWLock m_ipAddressLock;
        Poco::Mutex m_mutex;

        Poco::UInt32 m_msgId;

        void processPacket(const Receiver::ReceivedData& rd);
        void addIpAddress(const Receiver::ReceivedData& rd);
    };
}}
