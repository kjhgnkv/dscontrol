#pragma once

#include <string>
#include <map>
#include "ParserXML/ParserData.hpp"

namespace dsmonitor
{
typedef std::map<std::string, std::string> MapString;

struct ProcessData
{
public:
    // stores data process
    MapString process;
    // stores data thread
    std::vector<MapString> thread;
    // stores data xml
    ParserData xml;
};
}