#pragma once

#include <iostream>
#include <fstream>
#include <map>
#include <Poco/SharedPtr.h>
#include <Poco/Timestamp.h>
#include <Algorithms/checkSum.h>
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/ParseHandler.h>
#include <Poco/JSON/Stringifier.h>
#include <Poco/Thread.h>
#include <Poco/Mutex.h>

#include "DSHost/Protocol/Buffer.h"

#include "../Logger.h"

using DS::protocols::Buffer;

namespace DS {
namespace example {

struct IncomingMsg
{
    const std::string appType;
    const std::string block;
    const std::string msgType;

    static Poco::SharedPtr<IncomingMsg> parse(const Buffer& buffer)
    {
        if(buffer.empty())
        {
            Logger::warning("RawMessage is Null.");
            return Poco::SharedPtr<IncomingMsg>();
        }

        std::string json(buffer.begin(), buffer.end());
        try
        {
            Poco::JSON::Parser parser;
            Poco::Dynamic::Var result = parser.parse(json);
            Poco::JSON::Object::Ptr rootObj = result.extract<Poco::JSON::Object::Ptr>();
            std::string tblock;
            std::string tmsgType = rootObj->getValue<std::string>("msgType");
            if(!tmsgType.empty())
            {
                if(rootObj->has("block"))
                    tblock = rootObj->getValue<std::string>("block");
            }
            std::string tappType = rootObj->getValue<std::string>("appType");

            return new IncomingMsg(tmsgType, tblock, tappType);
        }
        catch(Poco::Exception& exc)
        {
            Logger::warning(std::string("can not parse received json: ") + exc.displayText());
        }

        return Poco::SharedPtr<IncomingMsg>();
    }
    ~IncomingMsg() { }

private:
    explicit IncomingMsg(const std::string& a_msgType, const std::string& a_block, const std::string& a_appType)
            : appType(a_appType)
            , block(a_block)
            , msgType(a_msgType)
    { }
};

}
}
