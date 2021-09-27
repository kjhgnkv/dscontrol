#pragma once
#include <string>
#include <vector>

namespace dsmonitor
{
// Data parser
struct ParserData
{
    void get(const ParserData &data)
    {
        key = data.key;
        path = data.path;
        xml = data.xml;
    }

    std::string key;
    std::string path;
    std::string xml;
};
}