#pragma once

#include <string>

namespace qmk
{

    class IConsole
    {
    public:
        enum class MessageType
        {
            BOOTLOADER,
            HID,
            COMMAND,
            INFO,
            ERROR
        };

        IConsole() {};
        virtual ~IConsole() {};

        virtual void Print(std::string text, MessageType msgType) = 0;
        virtual void PrintResponse(std::string text, MessageType msgType) = 0;

    protected:
        struct Message
        {
            std::string text;
            MessageType type;
            size_t level;
        };
    };

} // namespace qmk