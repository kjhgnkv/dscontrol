#include "global.h"

#include <string>
#include <QFile>

std::string getFileContent(const std::string& filename)
{
    QFile file{filename.c_str()};
    if (!file.open(QIODevice::ReadOnly))
    {
        //throw std::runtime_error{"can't open " + filename};
        //notify user?
        return "";
    }
    auto content = file.readAll();
    file.close();
    return content.toStdString();
}
