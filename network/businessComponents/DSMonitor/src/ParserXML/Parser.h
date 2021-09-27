#pragma once

#include "XML/xmlParser.h"
#include "DSServerGlobal.h"
#include "ParserData.hpp"

namespace dsmonitor
{
class Parser
{
public:
    bool parser(const std::string &xml);
    const std::vector<ParserData> &list() const;

private:
    bool deploy(const std::string &xml);
    bool update(const std::string &xml);
    bool stop(const std::string &xml);

    DS::dsserver::CommandType::Enum commandType(const std::string &xml);
    bool findKey(const std::string &findName, ParserData &findData);
    bool pathToNameApp(std::string &value);
    bool nameApp(std::string &value);
    bool trimNameApp(std::string &value, const std::string &data);
    bool pathApp(std::string &value);

private:
    std::vector<ParserData> list_;
    Poco::SharedPtr<ds::utils::XmlParser> xmlParser_;
};
}