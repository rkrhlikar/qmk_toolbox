#pragma once

#include <atomic>
#include <thread>

namespace qmk
{

    class ConsoleTextView;

    struct Device
    {
        std::string deviceName;
        std::string vendorId;
        std::string productId;
        std::string manufacturerName;
        std::string productName;
        std::string devPath = "";
    };

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