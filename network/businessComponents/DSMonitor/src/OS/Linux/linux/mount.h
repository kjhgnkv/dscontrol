#pragma once

#include <string>
#include <sys/statvfs.h>

#define MOUNTS "/proc/mounts"

namespace linuxapi
{
struct mount
{
    std::string device;
    std::string destination;
    std::string fstype; // filesystem type (ext2, ext3, ext4 ...)
    std::string options;
    int dump;
    int pass;

    uint64_t total;
    uint64_t available;
    uint64_t used;
    uint64_t free;
    double usage;
};

/* Put information about the application.  */
bool mount_info(struct mount* __mount, const char* __destination = "/")
{
    struct statvfs vfs = {};
    int stat = statvfs(__destination, &vfs);
    if (stat != 0)
    {
        return true;
    }

    __mount->total = vfs.f_blocks * vfs.f_frsize;
    __mount->available = vfs.f_bavail * vfs.f_frsize;
    __mount->free = vfs.f_bfree * vfs.f_frsize;
    __mount->used = __mount->total - __mount->free;

    uintmax_t u100 = __mount->used * 100;
    uintmax_t nonroot_total = __mount->used + __mount->available;

    __mount->usage = double(u100) / nonroot_total + (u100 % nonroot_total != 0);

    return false;
}

bool mount(struct mount* __mount, const std::string &__exe)
{
    std::ifstream ifmounts;
    ifmounts.open(MOUNTS);
    if (!ifmounts.is_open())
    {
        std::cerr << "Error opening \"" + std::string(MOUNTS) + "\" file!\n\t" << strerror(errno) << std::endl;
        return true;
    }

    int __count = 0;
    while (!ifmounts.eof())
    {
        struct mount firstMount = {};
        ifmounts >> firstMount.device >> firstMount.destination >> firstMount.fstype >> firstMount.options >> firstMount.dump >> firstMount.pass;
        if (!firstMount.device.empty())
        {
            std::size_t found = __exe.find(firstMount.destination);
            if (found != std::string::npos)
            {
                if (firstMount.destination.size() > __count)
                {
                    __count = firstMount.destination.size();
                    *__mount = firstMount;
                }
            }
        }
    }
    ifmounts.close();

    if (mount_info(__mount, __mount->destination.c_str()))
    {
        std::cerr << "Error: connect mount!" << std::endl;
        return true;
    }

    return false;
}
}