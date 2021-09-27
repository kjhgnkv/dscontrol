#pragma once

#include <map>
#include <string>

int create_directories(const std::string& path);

int install_file(const std::string& dest_path, const std::string& contents,
        const std::map<std::string, std::string>& dict = std::map<std::string, std::string>());

