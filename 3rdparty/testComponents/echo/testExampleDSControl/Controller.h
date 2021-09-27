#pragma once

#include <scopedPtr.h>
#include <Poco/AtomicCounter.h>

#include "Handler/MessageHandler.h"
#include "Handler/CommandHandler.h"

namespace DS {
namespace example {
    class Controller
    {
    public:
        explicit Controller(
                Sender::Ptr pSender
                , Receiver::Ptr pReceiver
                , EventService::Ptr pEventService
                , ProtocolService::Ptr pProtoService);
        ~Controller();

        bool init();
        bool exec();
        void stop();

        void onConnectionOpened(Poco::Dynamic::Var& data);
        void onConnectionClosed(Poco::Dynamic::Var& data);

    private:
        ds::utils::ScopedPtr<CommandHandler> m_commandHandler;

        EventService::Ptr m_pEventService;

        EventService::EventPtr m_pOpenEvent;
        EventService::EventPtr m_pCloseEvent;

        Poco::AtomicCounter m_isConnectionOpened, m_isStop;


    };
}
}
