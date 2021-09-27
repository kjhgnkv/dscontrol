#include "Parser.h"

#include <iostream>
#include <Poco/Path.h>

typedef std::vector<std::string> VecString;

#include "../../Logger.h"
using namespace DS::dsmonitor;

namespace dsmonitor
{
// функция для парсинга данных
// которые прислал DSServer
// function for parsing data
// sent by DSServer
bool Parser::parser(const std::string &xml)
{
    // error parser xml
    if (!ds::utils::XmlParser::parse(xml, xmlParser_))
    {
        Logger::error("read xml, " + xmlParser_->error());
        return true;
    }

    // get command xml
    switch (commandType(xml))
    {
        case DS::dsserver::CommandType::Deploy:
        {
            return deploy(xml);
        }
        case DS::dsserver::CommandType::Update:
        {
            return update(xml);
        }
        case DS::dsserver::CommandType::StopApplication:
        {
            return stop(xml);
        }
    }

    return true;
}

// get command type xml
DS::dsserver::CommandType::Enum Parser::commandType(const std::string &xml)
{
    VecString path;
    VecString values;

    path.clear();
    path.push_back("root");
    path.push_back("CommandType");

    if (!xmlParser_->getNodesValue(path, values))
    {
        path.clear();
        path.push_back("root");
        path.push_back("ApplicationPath");

        if (!xmlParser_->getNodesValue(path, values))
        {
            return DS::dsserver::CommandType::None;
        }

        return DS::dsserver::CommandType::Deploy;
    }

    return DS::dsserver::CommandType::fromString(*values.begin());
}

// deploy command
bool Parser::deploy(const std::string &xml)
{
    std::string appName;
    std::string appPath;

    // get app name
    if (!pathToNameApp(appName))
    {
        // get app path
        if (!pathApp(appPath))
        {
            ParserData findApp;
            // find app name
            if (!findKey(appName, findApp))
            {
                // update xml
                findApp.xml = xml;
                return false;
            }
            else
            {
                // insert xml
                ParserData root;
                root.key = appName;
                root.path = appPath;
                root.xml = xml;

                list_.push_back(root);

                return false;
            }
        }
    }

    Logger::error("Don't parser deploy\n");
    return true;
}

// deleting the xml if the application is found
bool Parser::stop(const std::string &xml)
{
    if (!list_.empty())
    {
        std::string findName;
        // find app name
        if (!pathToNameApp(findName))
        {
            std::vector<ParserData>::iterator it, end;
            for (it = list_.begin(), end = list_.end(); it != end; ++it)
            {
                ParserData findData = *it;
                if (findData.key == findName)
                {
                    // remove app
                    list_.erase(it);
                    return false;
                }
            }
        }
    }

    return true;
}

// update the xml if the app is found
bool Parser::update(const std::string &xml)
{
    std::string value;
    // get app name
    if (!nameApp(value))
    {
        ParserData findData;
        // find app name
        if (!findKey(value, findData))
        {
            // update xml
            findData.xml = xml;
            return false;
        }
        else
        {
            // deploy
            return deploy(xml);
        }
    }

    Logger::error("Don't update xml\n");
    return true;
}

// getting the name from the ApplicationName attribute
bool Parser::nameApp(std::string &value)
{
    VecString path;
    VecString values;

    path.push_back("root");
    path.push_back("ApplicationName");

    if (xmlParser_->getNodesValue(path, values))
    {
        VecString::const_iterator it = values.begin();
        if (it != values.end())
        {
            // get name
            value = *it;
            return false;
        }
    }

    return true;
}

// looking for a name in the list
bool Parser::findKey(const std::string &findName, ParserData &findData)
{
    if (!list_.empty())
    {
        std::vector<ParserData>::const_iterator it, end;
        for (it = list_.begin(), end = list_.end(); it != end; ++it)
        {
            findData = *it;
            if (findData.key == findName)
            {
                return false;
            }
        }
    }

    return true;
}

// if the ApplicationName attribute is missing,
// the name is considered ApplicationPath
bool Parser::pathToNameApp(std::string &value)
{
    VecString path;
    VecString values;

    path.push_back("root");
    path.push_back("ApplicationPath");

    if (xmlParser_->getNodesValue(path, values))
    {
        VecString::const_iterator it = values.begin();
        if (it != values.end())
        {
            std::string name = (*it);
            #ifdef  _WIN32
            int pos = name.find_last_of('\\');
            #elif __linux
            std::size_t pos = name.find_last_of('/');
            #endif
            if (pos != std::string::npos)
            {
                value = name.substr(pos + 1, name.size());
                return false;
            }
        }
    }

    return true;
}

// forming the path to the application
bool Parser::trimNameApp(std::string &value, const std::string &data)
{
    std::string nameApp;
    if (!pathToNameApp(nameApp))
    {
        #ifdef  _WIN32
        int count = std::count(data.begin(), data.end(), '\\');
        std::string current = Poco::Path::current();
        for (int i = 0; i < count + 1; i++)
        {
            int pos = current.find_last_of('\\');
            if (pos != std::string::npos)
            {
                current = current.substr(0, pos);
            }
        }
        value = current + "\\" + nameApp + "\\exe\\";
        #elif __linux
        value = data + "/exe/";
        #endif
        return false;
    }

    return true;
}

// getting the path to the bundle via the ApplicationPath attribute
bool Parser::pathApp(std::string &value)
{
    VecString path;
    VecString values;

    path.push_back("root");
    path.push_back("ApplicationPath");

    if (xmlParser_->getNodesValue(path, values))
    {
        VecString::const_iterator it = values.begin();
        if (it != values.end())
        {
            if (!trimNameApp(value, *it))
            {
                return false;
            }
        }
    }

    return true;
}

// returning the list of applications
const std::vector<ParserData> &Parser::list() const
{
    return list_;
}
}