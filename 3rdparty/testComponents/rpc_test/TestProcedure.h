#pragma once

#include "DSHost/RPC/Procedure.h"
#include <sstream>

using DS::protocols::RawMessage;

class TestProcedure : public DS::RPC::Procedure
{
public:
    std::string name() const { return "test_increment"; }

    void call(Message::Ptr pData, Callback::Ptr pCallback);
};

inline void TestProcedure::call(Message::Ptr pData, Callback::Ptr pCallback)
{
    if (!pCallback || !pData)
        return;

    RawMessage::Ptr pInput = pData.cast<RawMessage>();
    if (!pInput)
        pCallback->error(Callback::ERROR_MESSAGE_TYPE_UNSUPPORTED);

    std::istringstream iss(std::string(pInput->buffer().begin(), pInput->buffer().size()));
    int value;
    iss >> value;

    value++;

    std::ostringstream oss;
    oss << value;

    RawMessage::Ptr pOutput = pInput->clone().cast<RawMessage>();
    pOutput->buffer().assign(oss.str().c_str(), oss.str().length());

    pCallback->process(pOutput);
}

