#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

#include "iconsole.hpp"

namespace qmk
{

    class TerminalConsole : public IConsole
    {
    public:
        TerminalConsole();
        virtual ~TerminalConsole();

        void Print(std::string text, MessageType msgType);
        void PrintResponse(std::string text, MessageType msgType);

    private:
        std::thread printingThread_;
        std::mutex messageQueueLock_;
        std::queue<Message> messageQueue_;
        std::condition_variable messageConditionVar_;
        std::atomic<bool> stop_;
    };

} // namespace qmk