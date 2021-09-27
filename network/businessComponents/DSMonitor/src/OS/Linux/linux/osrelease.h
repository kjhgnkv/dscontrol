#pragma once

#include <sys/utsname.h>
#include <ostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <cstring>

#define PATH_OSRELEASE "/etc/os-release"

namespace linuxapi
{
struct osrelease
{
    // A pretty operating system name in a format suitable for presentation
    // to the user. May or may not contain a release code name or
    // OS version of some kind, as suitable. If not set, defaults
    // to "PRETTY_NAME="Linux"".
    // Example: "PRETTY_NAME="Fedora 17 (Beefy Miracle)"".
    char pretty_name[_UTSNAME_SYSNAME_LENGTH];

    // A string identifying the operating system, without a version
    // component, and suitable for presentation to the user. If not
    // set, defaults to "NAME=Linux".
    // Example: "NAME=Fedora" or "NAME="Debian GNU/Linux"".
    char name[_UTSNAME_SYSNAME_LENGTH];

    // A lower-case string (no spaces or other characters outside of
    // 0–9, a–z, ".", "_" and "-") identifying the operating system,
    // excluding any version information and suitable for processing
    // by scripts or usage in generated filenames. If not set,
    // defaults to "ID=linux". Example: "ID=fedora" or "ID=debian".
    char id[_UTSNAME_SYSNAME_LENGTH];

    // A lower-case string (mostly numeric, no spaces or other
    // characters outside of 0–9, a–z, ".", "_" and "-")
    // identifying the operating system version, excluding any
    // OS name information or release code name, and suitable
    // for processing by scripts or usage in generated filenames.
    // This field is optional.
    // Example: "VERSION_ID=17" or "VERSION_ID=11.04".
    char version_id[_UTSNAME_SYSNAME_LENGTH];

    // A lower-case string (no spaces or other characters outside of
    // 0–9, a–z, ".", "_" and "-") identifying the operating system
    // release code name, excluding any OS name information or release
    // version, and suitable for processing by scripts or usage in generated
    // filenames. This field is optional and may not be implemented
    // on all systems.
    // Examples: "VERSION_CODENAME=buster", "VERSION_CODENAME=xenial"
    char version_codename[_UTSNAME_SYSNAME_LENGTH];

    // A string identifying the operating system version, excluding any
    // OS name information, possibly including a release code name,
    // and suitable for presentation to the user. This field is optional.
    // Example: "VERSION=17" or "VERSION="17 (Beefy Miracle)"".
    char version[_UTSNAME_SYSNAME_LENGTH];

    friend std::ostream &operator<<(std::ostream &os, const osrelease &__osrelease)
    {
        os << "OS Release" << "\n" << "\tPRETTY_NAME: " << __osrelease.pretty_name << "\n" << "\tNAME: " << __osrelease.name << "\n" << "\tID: " << __osrelease.id << "\n" << "\tVERSION_ID: " << __osrelease.version_id << "\n" << "\tVERSION_CODENAME: " << __osrelease.version_codename << "\n" << "\tVERSION: " << __osrelease.version << "\n";

        return os;
    }
};

/* Put information about the application.  */
bool osrelease(struct osrelease* __osrelease)
{
    std::ifstream ifosrelease;
    ifosrelease.open(PATH_OSRELEASE);
    if (!ifosrelease.is_open())
    {
        std::cerr << "Error opening \"" + std::string(PATH_OSRELEASE) + "\" file!\n\t" << strerror(errno) << std::endl;
        return true;
    }

    std::vector<std::string> vec;
    std::vector<std::string>::iterator it;

    std::string line;
    while (getline(ifosrelease, line))
    {
        vec.push_back(line);
    }
    ifosrelease.close();

    if (vec.empty())
    {
        return true;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("PRETTY_NAME"));
    if (it != vec.end())
    {
        strcpy(__osrelease->pretty_name, substr(*it, '=').c_str());
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("NAME"));
    if (it != vec.end())
    {
        strcpy(__osrelease->name, substr(*it, '=').c_str());
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("ID"));
    if (it != vec.end())
    {
        strcpy(__osrelease->id, substr(*it, '=').c_str());
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("VERSION_ID"));
    if (it != vec.end())
    {
        strcpy(__osrelease->version_id, substr(*it, '=').c_str());
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("VERSION_CODENAME"));
    if (it != vec.end())
    {
        strcpy(__osrelease->version_codename, substr(*it, '=').c_str());
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("VERSION"));
    if (it != vec.end())
    {
        strcpy(__osrelease->version, substr(*it, '=').c_str());
    }

    return false;
}
}