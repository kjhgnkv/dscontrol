#include "Logger.h"

namespace DS
{
namespace dsmonitor
{
Poco::OSP::BundleContext::Ptr Logger::ms_pContex;
std::string Logger::ms_moduleName;
std::string Logger::ms_moduleVersion;
}
}
