#pragma once

#include <atomic>
#include <map>
#include <mutex>
#include <thread>

namespace qmk
{

    class IConsole;

    struct Device
    {
        enum class Chipset
        {
            DFU,
            HALFKAY,
            CATERINA,
            STM32,
            KIIBOHD,
            AVR_ISP,
            USB_ASP,
            USB_TINY,
            BOOTLOAD_HID,
            ATMEL_SAM_BA
        };

        std::string vendorId;
        std::string productId;
        std::string manufacturerName;
        std::string productName;
        std::string devPath = "";
        Chipset chipset;
    };

    class DeviceHandler
    {
    public:
        DeviceHandler();
        ~DeviceHandler();

        void Initialize(IConsole* console);

        void FlashConnectedDevices(const std::string& microcontroller, const std::string& filePath);

    private:
        IConsole* console_;

        std::atomic<bool> stop_;
        std::thread udevThread_;

        std::mutex devicesMapLock_;
        std::map<std::string, Device> devicesMap_;

        std::thread flashingThread_;
    };

} // namespace qmk