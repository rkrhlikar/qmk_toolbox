#pragma once

#include <atomic>
#include <thread>

namespace qmk
{

    class ConsoleTextView;
    struct Device;

    class DeviceHandler
    {
    public:
        DeviceHandler();
        ~DeviceHandler();

        void Initialize(ConsoleTextView* consoleTextView);

        void DeviceEventHandler(Device&& device, bool connected);

    private:
        ConsoleTextView* consoleTextView_;
        int callbackHandle_;

        std::atomic<bool> stop_;
        std::thread usbThread_;
    };

} // namespace qmk