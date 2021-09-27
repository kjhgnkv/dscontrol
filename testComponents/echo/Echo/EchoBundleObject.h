#pragma once 

#include "Poco/Runnable.h"
#include "Poco/AtomicCounter.h"
#include "Poco/Thread.h"
#include "Poco/Mutex.h"
#include "blocker.h"

#include "DSHost/Bundle/Object.h"
#include "DSHost/Communication/Service.h"
#include "DSHost/Communication/Sender.h"
#include "DSHost/Communication/Receiver.h"
#include "DSHost/Protocol/ImplementationFactoryService.h"

#define MIN_DESTINATION_ID 1
#define MAX_DESTINATION_ID 5

#define DEFAULT_CONTENT_ID "test_content"

#define SEND_PERIOD 3000 // ms

using DS::bundle_objects::BundleObject;
using DS::protocols::DefaultMessage;
using DS::communication::CommunicationService;
using DS::communication::Sender;
using DS::communication::Receiver;

class EchoBundleObject : public BundleObject, public Poco::Runnable
{
public:
    BUNDLE_OBJECT_CTOR_DECL(EchoBundleObject);

    void run();

protected:
    void log(const std::string& info, bool warn = false);

    void onInitialize();
    void onStart();
    void onStop();
    void onUninitialize();

    void onSystemEvent(Poco::OSP::SystemEvents::EventKind& eventKind);

    void onReceive(const Receiver::ReceivedMessage& msg);

private:
    std::string _version;

    DefaultMessage::Ptr _pData;
    std::string _contentId;

    CommunicationService::Ptr _pCommService;
    Sender::Ptr _pSender;
    Receiver::Ptr _pReceiver;

    Poco::Thread _thread;
    Poco::AtomicCounter _flag, _stopped;
    ds::utils::Blocker _blocker;

    bool _hide_warnings;

    void prepareXMLCompatibleMessage(const std::string text);
    void prepareStdBinCompatibleMessage(const std::string text);
    static std::string stringifyNameList(const std::set<std::string>& names);
};

///////////////////////////////////////////////////////////////////////////////

inline void EchoBundleObject::log(const std::string& info, bool warn)
{
    std::string full_text = "\"";
    full_text += name();
    full_text += "\" v" + _version + ": " + info;

    if (!warn)
        context()->logger().information(full_text);
    else if (!_hide_warnings)
        context()->logger().warning(full_text);
    else
        context()->logger().debug(full_text);
}

inline void EchoBundleObject::onInitialize()
{
    log("onInitialize()");

    _pSender = _pCommService->sender(name());
    _pReceiver = _pCommService->receiver(name());
    _pReceiver->reception += delegate(this, &EchoBundleObject::onReceive);
}

inline void EchoBundleObject::onStart()
{
    log("onStart()");

    if (_pSender && !_thread.isRunning())
    {
        _flag = 0;
        _thread.start(*this);
    }
}

inline void EchoBundleObject::onStop()
{
    log("onStop()");

    _flag = 1;
    _blocker.unblock();
    _thread.join();
}

inline void EchoBundleObject::onUninitialize()
{
    log("onUninitialize()");

    _pReceiver->reception -= delegate(this, &EchoBundleObject::onReceive);
    _pReceiver.reset();
    _pSender.reset();
}

inline void EchoBundleObject::onSystemEvent(Poco::OSP::SystemEvents::EventKind& eventKind)
{
    switch (eventKind)
    {
        case Poco::OSP::SystemEvents::EV_SYSTEM_STARTED:
            log("onSystemEvent(): EV_SYSTEM_STARTED");
            break;

        case Poco::OSP::SystemEvents::EV_SYSTEM_SHUTTING_DOWN:
            log("onSystemEvent(): EV_SYSTEM_SHUTTING_DOWN");
            break;
    }
}

