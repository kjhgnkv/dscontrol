#include "installer.h"
#include <fstream>
#include <filesystem>
#include <regex>
#include <iostream>

int create_directories(const std::string& path)
{
    std::error_code ec;
    if (!std::filesystem::create_directories(path, ec) && ec)
    {
        std::cout << "Error creating directory \"" << path
            << "\", error code = " << ec << "\n";
        return 1;
    }

    std::cout << "Created directory \"" << path << "\"\n";
    return 0;
}

int install_file(const std::string& dest_path, const std::string& contents,
        const std::map<std::string, std::string>& dict)
{
    std::string result(contents);

    for (const auto& [key, val] : dict)
        result = std::regex_replace(result, std::regex(std::string("%") + key + "%"), val);

    try
    {
        std::ofstream fout;
        fout.open(dest_path);

        fout << result;

        fout.close();
    }
    catch (...)
    {
        std::cout << "Error writing file \"" << dest_path << "\"\n";
        return 1;
    }

    std::cout << "Written file \"" << dest_path << "\"\n";
    return 0;
}

