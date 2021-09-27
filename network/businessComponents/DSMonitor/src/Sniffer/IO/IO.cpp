#include <iostream>
#include <cstdlib>

#include "../core/sockets.h"
#include "IO.h"

void IO::print(const std::string &line)
{
    std::cout << line << " ";
}

void IO::println(const std::string &line)
{
    std::cout << line << std::endl;
}

void IO::print_err(const std::string &line)
{
    std::cerr << line << std::endl;
}

void IO::print_help()
{
    println("Usage of sniffer: \n"
            "[-h| --help] - println this message \n"
            "[-d| --device name] - use some device to listen\n"
            "[-f| --file name] - redirect to file in specific format \n"
            "[-ff| --from-file name] - output file \n"
            "[-a| --amount number] - number of packages to handle (-1 - without limit) \n"
            "[-o| --out] - write to file and stdio (if --file present) \n"
            "[-c| --console] - println into stdout (gui by default) \n"
            "[-p| --protocol name] - to filter using protocol name(doesn't work for reading from file)");

    exit(EXIT_SUCCESS);
}

void IO::print_socket(const std::vector<sockets::detail_view> &view)
{
    std::vector<sockets::detail_view>::const_iterator it, end;
    for (it = view.begin(), end = view.end(); it != end; it++)
    {
        print_socket(*it);
    }

    IO::println("------");
}

void IO::print_socket(const sockets::detail_view &detail_view)
{
    IO::println(detail_view.preview);

    // sockets::VecPairString::const_iterator it, end;
    sockets::MapString::const_iterator it, end;
    for (it = detail_view.properties.begin(), end = detail_view.properties.end(); it != end; it++)
    {
        printf("\t%-20s\t: %-20s\n", it->first.c_str(), it->second.c_str());
    }
}