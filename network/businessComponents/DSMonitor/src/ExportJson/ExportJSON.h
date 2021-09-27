#pragma once

#include <Poco/JSON/Object.h>
#include <vector>

namespace dsmonitor
{
struct MonitorData;
struct ParserData;

typedef std::map<std::string, std::string> MapString;

class ExportJSON
{
private:
    typedef Poco::JSON::Object JsonObj;
    typedef Poco::JSON::Array JsonArray;

public:
    static std::string exportJsonFile(const MonitorData &data) ;

private:
    static void addObjectPtr(JsonObj::Ptr &prt, const MapString &data);
    static void addObjectPtr(JsonObj::Ptr &prt, const std::string &key, const Poco::Dynamic::Var &value);
};
}