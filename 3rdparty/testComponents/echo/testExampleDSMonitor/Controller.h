#pragma once

#include <scopedPtr.h>
#include <Poco/AtomicCounter.h>

#include "Handler/MessageHandler.h"

namespace DS {
namespace example {
namespace dsmonitor {
    class Controller
    {
    public:
        explicit Controller(
                Sender::Ptr pSender
                , Receiver::Ptr pReceiver
                , ProtocolsImplementationFactoryService::Ptr pImplementationFactoryService
                , ProtocolService::Ptr pProtoService);
        ~Controller();

        bool init();
        bool exec();
        void stop();

    private:
        Poco::AtomicCounter m_isStop;

        ds::utils::ScopedPtr<MessageHandler> m_messageHandler;


    };
}
}}
