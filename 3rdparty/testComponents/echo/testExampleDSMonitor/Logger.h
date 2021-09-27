#pragma once 

#include "Poco/OSP/BundleContext.h"

namespace DS {
namespace example {
namespace dsmonitor {

class Logger
{
public:
    static void init(Poco::OSP::BundleContext::Ptr pContex, const std::string& name, const std::string& version)
    {
        ms_pContex = pContex;
        ms_moduleName = name;
        ms_moduleVersion = version;
    }

    static void warning(const std::string& msg)
    {
        ms_pContex->logger().warning(ms_moduleName + " (v" + ms_moduleVersion + "): " + msg);
    }

    static void error(const std::string& msg)
    {
        ms_pContex->logger().error(ms_moduleName + " (v" + ms_moduleVersion + "): " + msg);
    }

    static void information(const std::string& msg)
    {
        ms_pContex->logger().information(ms_moduleName + " (v" + ms_moduleVersion + "): " + msg);
    }

    static void notice(const std::string& msg)
    {
        ms_pContex->logger().notice(ms_moduleName + " (v" + ms_moduleVersion + "): " + msg);
    }

    static void debug(const std::string& msg)
    {
        ms_pContex->logger().debug(ms_moduleName + " (v" + ms_moduleVersion + "): " + msg);
    }


private:
    static Poco::OSP::BundleContext::Ptr ms_pContex;
    static std::string ms_moduleName;
    static std::string ms_moduleVersion;
};

}}}
