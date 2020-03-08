#include "device_handler.hpp"

#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <string>

#include <sys/time.h>
#include <libudev.h>

#include "console_text_view.hpp"

namespace qmk
{

    class UdevDevice
    {
    public:
        UdevDevice(struct udev_device* device) : device_(device) {}

        ~UdevDevice()
        {
            udev_device_unref(device_);
        }

        operator struct udev_device*() const
        {
            return device_;
        }

        operator bool() const
        {
            return device_;
        }

    private:
        struct udev_device* device_;
    };

    static std::string ChipsetToString(Device::Chipset chipset)
    {
        switch(chipset)
        {
            case Device::Chipset::DFU:
                return "DFU";
            case Device::Chipset::HALFKAY:
                return "Halfkay";
            case Device::Chipset::CATERINA:
                return "Caterina";
            case Device::Chipset::STM32:
                return "STM32";
            case Device::Chipset::KIIBOHD:
                return "Kiibohd";
            case Device::Chipset::AVR_ISP:
                return "AVRISP";
            case Device::Chipset::USB_ASP:
                return "USBAsp";
            case Device::Chipset::USB_TINY:
                return "USB Tiny";
            case Device::Chipset::BOOTLOAD_HID:
                return "BootloadHID";
            case Device::Chipset::ATMEL_SAM_BA:
                return "Atmel SAM-BA";
            default:
                throw std::runtime_error("Unknown chipset");
        }
    }

    DeviceHandler::DeviceHandler() {}

    void DeviceHandler::Initialize(ConsoleTextView* consoleTextView)
    {
        consoleTextView_ = consoleTextView;

        stop_ = false;

        udevThread_ = std::thread([this]() {
            struct udev* udev = udev_new();
            if(!udev) throw std::runtime_error("Can't create udev");

            struct udev_monitor* monitor = udev_monitor_new_from_netlink(udev, "udev");
            if(!monitor) throw std::runtime_error("Can't create udev monitor");

            udev_monitor_filter_add_match_subsystem_devtype(monitor, "usb", nullptr);
            udev_monitor_filter_add_match_subsystem_devtype(monitor, "tty", nullptr);
            udev_monitor_enable_receiving(monitor);

            int fd = udev_monitor_get_fd(monitor);

            while(!stop_)
            {
                fd_set fds;
                FD_ZERO(&fds);
                FD_SET(fd, &fds);

                struct timeval tv;
                tv.tv_sec = 0;
                tv.tv_usec = 0;

                int ready = select(fd+1, &fds, NULL, NULL, &tv);
                if (ready > 0 && FD_ISSET(fd, &fds))
                {
                    UdevDevice udevDevice = udev_monitor_receive_device(monitor);
                    if (udevDevice)
                    {
                        std::string action = udev_device_get_action(udevDevice);

                        std::string sysPath = udev_device_get_syspath(udevDevice);

                        Device device;

                        if(action == "add")
                        {
                            std::string subsystem = udev_device_get_subsystem(udevDevice);

                            const char* manufacturerName = udev_device_get_property_value(udevDevice, "manufacturer");
                            const char* productName = udev_device_get_property_value(udevDevice, "product");
                            if(manufacturerName == nullptr) manufacturerName = "";
                            if(productName == nullptr) productName = "";

                            device.manufacturerName = manufacturerName;
                            device.productName = productName;

                            if(subsystem == "usb")
                            {
                                // Check for devices not programmed using avrdude (don't require dev path)
                                const char* vendor = udev_device_get_sysattr_value(udevDevice, "idVendor");
                                const char* product = udev_device_get_sysattr_value(udevDevice, "idProduct");
                                if(!vendor || !product) continue;

                                device.vendorId = vendor;
                                device.productId = product;

                                int vendorId = std::stoi(std::string(vendor), nullptr, 16);
                                int productId = std::stoi(std::string(product), nullptr, 16);
                                
                                switch(vendorId)
                                {
                                    case 0x03EB:
                                    {
                                        // Atmel - DFU
                                        device.chipset = Device::Chipset::DFU;
                                        break;
                                    }
                                    case 0x16C0:
                                    {
                                        switch(productId)
                                        {
                                            case 0x05DF:
                                            {
                                                // Objective Development
                                                device.chipset = Device::Chipset::BOOTLOAD_HID;
                                                break;
                                            }
                                            case 0x0478:
                                            {
                                                // PJRC
                                                device.chipset = Device::Chipset::HALFKAY;
                                                break;
                                            }
                                            default:
                                            {
                                                // Other devices
                                                continue;
                                            }
                                        }

                                        break;
                                    }
                                    case 0x0483:
                                    { 
                                        switch(productId)
                                        {
                                            case 0xDF11:
                                            {
                                                // STM32
                                                device.chipset = Device::Chipset::STM32;
                                                break;
                                            }
                                            default:
                                            {
                                                // Other devices
                                                continue;
                                            }
                                        }
                                        
                                        break;
                                    }
                                    case 0x1C11:
                                    {
                                        switch(productId)
                                        {
                                            case 0xB007:
                                            {
                                                // Kiibohd
                                                device.chipset = Device::Chipset::KIIBOHD;
                                            }
                                            default:
                                            {
                                                // Other devices
                                                continue;
                                            }
                                        }

                                        break;
                                    }
                                    default:
                                    {
                                        // Other devices
                                        continue;
                                    }
                                }
                            }
                            else if(subsystem == "tty")
                            {
                                // Check for devices programmed using avrdude
                                const char* devNode = udev_device_get_devnode(udevDevice);
                                if(!devNode) continue;

                                device.devPath = devNode;
                                
                                const char* vendor = udev_device_get_property_value(udevDevice, "ID_VENDOR_ID");
                                const char* product = udev_device_get_property_value(udevDevice, "ID_MODEL_ID");
                                if(!vendor || !product) continue;

                                device.vendorId = vendor;
                                device.productId = product;

                                int vendorId = std::stoi(std::string(vendor), nullptr, 16);
                                int productId = std::stoi(std::string(product), nullptr, 16);
                                
                                switch(vendorId)
                                {
                                    case 0x03EB:
                                    {
                                        // Atmel
                                        switch(productId)
                                        {
                                            case 0x6124:
                                            {
                                                // SAM-BA
                                                device.chipset = Device::Chipset::ATMEL_SAM_BA;
                                                break;
                                            }
                                            default:
                                            {
                                                // Other devices
                                                continue;
                                            }
                                        }

                                        break;
                                    }
                                    case 0x2341:
                                    {
                                        // Arduino
                                        device.chipset = Device::Chipset::CATERINA;
                                        break;
                                    }
                                    case 0x1B4F:
                                    {
                                        // Sparkfun
                                        device.chipset = Device::Chipset::CATERINA;
                                        break;
                                    }
                                    case 0x239A:
                                    {
                                        // Adafruit
                                        device.chipset = Device::Chipset::CATERINA;
                                        break;
                                    }
                                    case 0x16C0:
                                    {
                                        switch(productId)
                                        {
                                            case 0x0483:
                                            {
                                                // Arduino ISP
                                                device.chipset = Device::Chipset::AVR_ISP;
                                                break;
                                            }
                                            case 0x05DC:
                                            {
                                                // AVR USBAsp
                                                device.chipset = Device::Chipset::USB_ASP;
                                                break;
                                            }
                                            default:
                                            {
                                                // Other devices
                                                continue;
                                            }
                                        }

                                        break;
                                    }
                                    case 0x1781:
                                    {
                                        switch(productId)
                                        {
                                            case 0x0C9F:
                                            {
                                                // AVR Pocket ISP
                                                device.chipset = Device::Chipset::USB_TINY;
                                                break;
                                            }
                                            default:
                                            {
                                                // Other devices
                                                continue;
                                            }
                                        }

                                        break;
                                    }
                                    default:
                                    {
                                        // Other devices
                                        continue;
                                    }
                                }
                            }

                            {
                                std::lock_guard<std::mutex> lock(devicesMapLock_);
                                devicesMap_[sysPath] = device;
                            }
                        }
                        else if(action == "remove")
                        {
                            auto deviceIt = devicesMap_.find(sysPath);
                            if(deviceIt == devicesMap_.end()) continue;

                            device = deviceIt->second;

                            {
                                std::lock_guard<std::mutex> lock(devicesMapLock_);
                                devicesMap_.erase(deviceIt);
                            }
                        }
                        else
                        {
                            // Other events
                            continue;
                        }
                        
                        std::stringstream message;
                        message << ChipsetToString(device.chipset) << " device ";
                        message << ((action == "add") ? "connected":"disconnected") << " ";
                        message << device.manufacturerName << " "<< device.productName;
                        message << " (" << device.vendorId << ":" << device.productId << ")";
                        if(device.devPath != "") message << " @ " << std::string(device.devPath);
                        consoleTextView_->Print(message.str(), ConsoleTextView::MessageType::BOOTLOADER);
                    }
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }

            udev_monitor_unref(monitor);
            udev_unref(udev);
        });
    }

    DeviceHandler::~DeviceHandler()
    {
        stop_ = true;
        udevThread_.join();
    }

    void DeviceHandler::FlashConnectedDevices(const std::string& microcontroller, const std::string& filePath)
    {
        if(flashingThread_.joinable()) flashingThread_.join();

        flashingThread_ = std::thread([this, microcontroller, filePath]() {
            std::lock_guard<std::mutex> devicesLock(devicesMapLock_);

            for(const std::pair<std::string, Device>& devicePair : devicesMap_)
            {
                const Device& device = std::get<1>(devicePair);

                switch(device.chipset)
                {
                    case Device::Chipset::DFU:
                    {
                        break;
                    }
                    case Device::Chipset::HALFKAY:
                    {
                        break;
                    }
                    case Device::Chipset::CATERINA:
                    {
                        std::string args = "-C avrdude.conf -p " + microcontroller + " -c avr109 -U flash:w:\"" + filePath + "\":i -P " + device.devPath;
                        consoleTextView_->Print("avrdude " + args, ConsoleTextView::MessageType::COMMAND);

                        std::array<char, 256> buffer;
                        std::stringstream outputStream;

                        std::string command = "./bin/avrdude " + args + " 2>&1";

                        FILE* pipe = popen(command.c_str(), "r");
                        if(!pipe) throw std::runtime_error("Failed to run command");

                        while(fgets(buffer.data(), buffer.size(), pipe) != NULL)
                        {
                            outputStream << buffer.data();
                            std::string line;
                            std::getline(outputStream, line);
                            if(line != "")
                            {
                                line += "\n";
                                consoleTextView_->PrintResponse(line, ConsoleTextView::MessageType::INFO);
                                
                                if (line.find("Bootloader and code overlap.") != std::string::npos || // DFU
                                    line.find("exceeds remaining flash size!") != std::string::npos || // BootloadHID
                                    line.find("Not enough bytes in device info report") != std::string::npos) // BootloadHID
                                {
                                    consoleTextView_->Print("File is too large for device", ConsoleTextView::MessageType::ERROR);
                                }
                            }
                        }

                        pclose(pipe);

                        break;
                    }
                    case Device::Chipset::STM32:
                    {
                        break;
                    }
                    case Device::Chipset::KIIBOHD:
                    {
                        break;
                    }
                    case Device::Chipset::AVR_ISP:
                    {
                        break;
                    }
                    case Device::Chipset::USB_ASP:
                    {
                        break;
                    }
                    case Device::Chipset::USB_TINY:
                    {
                        break;
                    }
                    case Device::Chipset::BOOTLOAD_HID:
                    {
                        break;
                    }
                    case Device::Chipset::ATMEL_SAM_BA:
                    {
                        break;
                    }
                    default:
                        throw std::runtime_error("Unknown chipset");
                }
            }
        });
    }

} // namespace qmk