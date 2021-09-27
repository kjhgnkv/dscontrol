#pragma once 

#include "Poco/Runnable.h"
#include "Poco/AtomicCounter.h"
#include "Poco/Thread.h"
#include "Poco/Mutex.h"
#include "blocker.h"

#include "DSHost/Bundle/Object.h"
#include "DSHost/Bundle/ObjectService.h"
#include "DSHost/Protocol/Raw/Protocol.h"
#include "DSHost/RPC/Object.h"
#include "DSHost/RPC/Service.h"

#define RPC_BUNDLE_OBJECT_NAME "rpc"
#define PORT_NAME "comm"
#define PROTOCOL_NAME "integer"
#define PACKER_UNPACKER_OWNER "ds.host.samples.rpc_test"

#define SEND_PERIOD 5000 // ms

using DS::bundle_objects::BundleObject;
using DS::bundle_objects::BundleObjectService;
using DS::protocols::RawProtocol;
using DS::RPC::Procedure;
using DS::RPC::RPCBundleObject;
using DS::RPC::RPCService;

class RPCTestBundleObject : public BundleObject, public Poco::Runnable
{
public:
    BUNDLE_OBJECT_CTOR_DECL(RPCTestBundleObject);

    void run();

protected:
    void log(const std::string& info, bool warn = false);

    void onInitialize();
    void onStart();
    void onStop();
    void onUninitialize();

private:
    RawProtocol::Ptr _pRawProtocol;
    RPCService::Ptr _pRPCService;

    Procedure::Ptr _pLocalProcedure;
    bool _dontRegister;

    BundleObjectService::Ptr _pBundleObjectService;

    Poco::Thread _thread;
    Poco::AtomicCounter _flag, _stopped;
    ds::utils::Blocker _blocker;
};

///////////////////////////////////////////////////////////////////////////////

inline void RPCTestBundleObject::log(const std::string& info, bool warn)
{
    std::string full_text = "RPC test \"";
    full_text += name() + "\": " + info;

    if (!warn)
        context()->logger().information(full_text);
    else
        context()->logger().warning(full_text);
}

inline void RPCTestBundleObject::onInitialize()
{
    log("onInitialize()");

    if (!_dontRegister)
        _pRPCService->registerLocalProcedure(_pLocalProcedure);
}

inline void RPCTestBundleObject::onStart()
{
    log("onStart()");

    if (!_thread.isRunning())
    {
        _flag = 0;
        _thread.start(*this);
    }
}

inline void RPCTestBundleObject::onStop()
{
    log("onStop()");

    _flag++;
    _thread.join();
}

inline void RPCTestBundleObject::onUninitialize()
{
    log("onUninitialize()");

    if (!_dontRegister)
        _pRPCService->unregisterLocalProcedure(_pLocalProcedure->name());
}

