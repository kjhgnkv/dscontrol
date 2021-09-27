#pragma once

#include <memory>
#include <string>

class IncomingLocalMessage
{
public:
    static bool parse(const std::string &json, std::shared_ptr<IncomingLocalMessage> &msg);
    ~IncomingLocalMessage() = default;

    const std::string appType() const;
    const std::string block() const;
    const std::string msgType() const;
    const std::string error() const;
    int errorId() const;
    const std::string data() const;
    const std::string json() const;

private:
    explicit IncomingLocalMessage(const std::string &msgType, const std::string &block, const std::string &appType,
                                  const std::string &error, int errorId, const std::string &data,
                                  const std::string &json);

private:
    const std::string appType_;
    const std::string block_;
    const std::string msgType_;
    const std::string error_;
    const int errorId_;
    const std::string data_;
    const std::string json_;
};
