#include <fstream>
#include <vector>

#include "../config.h"
#include "../utils/utils.h"

std::vector<std::string> utils::get_recent_files()
{
    std::ifstream file(FILE_RECENT_LOGS);
    if (!file.is_open())
    {
        return std::vector<std::string>();
    }

    std::vector<std::string> res;
    std::string line;

    while (std::getline(file, line))
    {
        res.push_back(line);
    }

    file.close();
    return res;
}

void utils::set_recent_file(const std::string &file_name)
{
    std::vector<std::string>::const_iterator it, end;
    std::vector<std::string> recentFiles = get_recent_files();

    for (it = recentFiles.begin(), end = recentFiles.end(); it != end; it++)
    {
        std::string file_name_saved = *it;
        if (file_name_saved == file_name)
        {
            return;
        }
    }

    std::ofstream file(FILE_RECENT_LOGS, std::ifstream::app);

    file << file_name << "\n";

    file.close();
}