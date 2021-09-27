#pragma once

#include "DSHost/App/Service.h"
#include <QObject>

%namespace-begin%

class %name%BundleObject : public QObject, public DS::bundle_objects::BundleObject
{
    Q_OBJECT

public:
    BUNDLE_OBJECT_CTOR_DECL(%name%BundleObject);

    // TODO: declare object interface here

protected:
    void onInitialize();
    void onStart();
    void onStop();
    void onUninitialize();

    void onSystemEvent(Poco::OSP::SystemEvents::EventKind& eventKind);

    void onReceive(const DS::communication::Receiver::ReceivedMessage& msg);

private:
    DS::app::AppService::Ptr _pAppService;

    DS::communication::Sender::Ptr _pSender;
    DS::communication::Receiver::Ptr _pReceiver;

    std::string _version;
};

%namespace-end%

