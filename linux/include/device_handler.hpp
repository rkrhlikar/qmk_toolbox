#pragma once

#include <atomic>
#include <thread>

namespace qmk
{

    class ConsoleTextView;

    class DeviceHandler
    {
    public:
        DeviceHandler();
        ~DeviceHandler();

        void Initialize(ConsoleTextView* consoleTextView);

    private:
        ConsoleTextView* consoleTextView_;

        std::atomic<bool> stop_;
        std::thread udevThread_;
    };

} // namespace qmk