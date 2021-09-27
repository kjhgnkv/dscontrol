#pragma once

#include <string>
#include <iostream>
#include <dirent.h>
#include <fnmatch.h>

namespace linuxapi
{
std::string substr(const std::string &str, char sub = ':')
{
    const std::string whitespace = " \t";

    std::string t = str.substr(str.find(sub) + 1);

    const int strBegin = t.find_first_not_of(whitespace);
    const int strEnd = t.find_last_not_of(whitespace);
    const int strRange = strEnd - strBegin + 1;

    return t.substr(strBegin, strRange);
}

struct FindStr
{
    FindStr(const std::string &str)
    : str_(str)
    {
    }

    bool operator()(const std::string &str)
    {
        return str.find(str_) != std::string::npos;
    }

private:
    std::string str_;
};

bool dirCount(const std::string &path, const std::string &pattern, int &count)
{
    DIR* sys_dir = opendir(path.c_str());
    if (sys_dir == NULL)
    {
        // std::cout << "Cannot open " << path << " directory." << std::endl;
        return true;
    }

    const struct dirent* dir;
    while ((dir = readdir(sys_dir)) != NULL)
    {
        if (fnmatch(std::string(pattern + "[0-9]*").c_str(), dir->d_name, 0) != 0)
        {
            continue;
        }
        count++;
    }

    if (closedir(sys_dir))
    {
        // std::cout << "Cannot close " << path << " directory." << std::endl;
        return true;
    }

    return false;
}
}