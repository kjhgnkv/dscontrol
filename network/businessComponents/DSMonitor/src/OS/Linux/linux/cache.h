#pragma once

#include <Poco/Dynamic/Var.h>

#include <sys/utsname.h>
#include <vector>
#include <iostream>

#define CACHE "/sys/devices/system/cpu/cpu0/cache"
#define CACHE_INDEX "index"

#include "../../../DSMonitorHelp.hpp"
#include "linuxhelp.hpp"

namespace linuxapi
{
struct cache_info
{
    unsigned int level;
    char type[_UTSNAME_LENGTH];
    unsigned int size;
    unsigned int sets;
    unsigned int way;
};

struct cache
{
    std::vector<cache_info> cache_list;

    friend std::ostream &operator<<(std::ostream &os, const cache &__cache)
    {
        std::vector<cache_info>::const_iterator it, end;

        for (it = __cache.cache_list.begin(), end = __cache.cache_list.end(); it != end; it++)
        {
            struct cache_info __cache_info = *it;
            os << "InfoCache:" << "\n" << "\tlevel: " << __cache_info.level << "\n" << "\ttype: " << __cache_info.type << "\n" << "\tsize: " << __cache_info.size << "\n" << "\tway: " << __cache_info.way << "\n" << "\tsets: " << __cache_info.sets << "\n";
        }

        return os;
    }
};

/* Put information about the cacheinfo.  */
bool cache(struct cache* __cache)
{
    int count = 0;
    if (!dirCount(CACHE, CACHE_INDEX, count))
    {
        for (int index = 0; index < count; index++)
        {
            std::string path = std::string(CACHE) + "/index" + Poco::Dynamic::Var(index).toString() + "/";

            std::ifstream iflevel;
            iflevel.open(std::string(path + "level").c_str());
            if (!iflevel.is_open())
            {
                std::cerr << "Error opening \"" + std::string(path) + " level\" file!\n\t" << strerror(
                errno) << std::endl;
                return true;
            }

            cache_info info;
            iflevel >> info.level;
            iflevel.close();

            std::ifstream iftype;
            iftype.open(std::string(path + "type").c_str());
            if (!iftype.is_open())
            {
                std::cerr << "Error opening \"" + std::string(path) + " type\" file!\n\t" << strerror(
                errno) << std::endl;
                return true;
            }
            iftype >> info.type;
            iftype.close();

            std::ifstream ifsize;
            ifsize.open(std::string(path + "size").c_str());
            if (!ifsize.is_open())
            {
                std::cerr << "Error opening \"" + std::string(path) + " size\" file!\n\t" << strerror(
                errno) << std::endl;
                return true;
            }
            ifsize >> info.size;
            ifsize.close();

            std::ifstream ifsets;
            ifsets.open(std::string(path + "number_of_sets").c_str());
            if (!ifsets.is_open())
            {
                std::cerr << "Error opening \"" + std::string(path) + " number_of_sets\" file!\n\t" << strerror(
                errno) << std::endl;
                return true;
            }
            ifsets >> info.sets;
            ifsets.close();

            std::ifstream ifway;
            ifway.open(std::string(path + "ways_of_associativity").c_str());
            if (!ifway.is_open())
            {
                std::cerr << "Error opening \"" + std::string(path) + " ways_of_associativity\" file!\n\t" << strerror(
                errno) << std::endl;
                return true;
            }
            ifway >> info.way;
            ifway.close();

            __cache->cache_list.push_back(info);
        }

        return false;
    }

    return true;
}
}