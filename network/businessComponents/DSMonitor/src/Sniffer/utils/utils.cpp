#include <string>
#include <cstdlib>
// #include <boost/regex.hpp>

#include "utils.h"

bool utils::is_integer(const std::string &s)
{
    // TODO
    // ERROR
    // return boost::regex_match(s, boost::regex("[0-9]+"));
    return false;
}

int utils::to_number(const std::string &s)
{
    char* end;
    if (is_integer(s))
    {
        return std::strtol(s.c_str(), &end, 10);
    }
    return 0;
}