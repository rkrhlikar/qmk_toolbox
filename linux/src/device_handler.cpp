#include "device_handler.hpp"

#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>

#include <sys/time.h>
#include <libudev.h>

#include "console_text_view.hpp"

namespace qmk
{

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
                    struct udev_device* device = udev_monitor_receive_device(monitor);
                    if (device)
                    {
                        std::string action = udev_device_get_action(device);

                        if(action == "add" || action == "remove")
                        {
                            std::string subsystem = udev_device_get_subsystem(device);

                            const char* manufacturerName = udev_device_get_property_value(device, "manufacturer");
                            const char* productName = udev_device_get_property_value(device, "product");
                            if(manufacturerName == nullptr) manufacturerName = "";
                            if(productName == nullptr) productName = "";

                            if(subsystem == "usb")
                            {
                                // Check for devices not programmed using avrdude (don't require dev path)
                                const char* vendor = udev_device_get_sysattr_value(device, "idVendor");
                                const char* product = udev_device_get_sysattr_value(device, "idProduct");
                                if(!vendor || !product) continue;

                                int vendorId = std::stoi(std::string(vendor), nullptr, 16);
                                int productId = std::stoi(std::string(product), nullptr, 16);

                                std::string deviceName = "";
                                
                                switch(vendorId)
                                {
                                    case 0x03EB:
                                    {
                                        // Atmel - DFU
                                        deviceName = "DFU";
                                        break;
                                    }
                                    case 0x16C0:
                                    {
                                        switch(productId)
                                        {
                                            case 0x05DF:
                                            {
                                                // Objective Development
                                                deviceName = "BootloadHID";
                                                break;
                                            }
                                            case 0x0478:
                                            {
                                                // PJRC
                                                deviceName = "Halfkay";
                                                break;
                                            }
                                        }

                                        break;
                                    }
                                    case 0x0483:
                                    { 
                                        if(productId == 0xDF11)
                                        {
                                            // STM32
                                            deviceName = "STM32";
                                        }
                                        
                                        break;
                                    }
                                    case 0x1C11:
                                    {
                                        if(productId == 0xB007)
                                        { 
                                            // Kiibohd
                                            deviceName = "Kiibohd";
                                        }

                                        // Other devices
                                        break;
                                    }
                                }

                                if(deviceName != "")
                                {
                                    std::stringstream message;
                                    message << deviceName << " device " << ((action == "add") ? "connected":"disconnected") << " ";
                                    message << std::string(manufacturerName) + " " + std::string(productName) + " (";
                                    message << std::setfill('0') << std::setw(4) << std::hex << vendorId;
                                    message << ":";
                                    message << std::setw(4) << productId;
                                    message << ")";
                                    consoleTextView_->Print(message.str(), ConsoleTextView::MessageType::BOOTLOADER);
                                }
                            }
                            else if(subsystem == "tty")
                            {
                                // Check for devices programmed using avrdude
                                const char* devNode = udev_device_get_devnode(device);
                                if(devNode)
                                {
                                    const char* vendor = udev_device_get_property_value(device, "ID_VENDOR_ID");
                                    const char* product = udev_device_get_property_value(device, "ID_MODEL_ID");
                                    if(!vendor || !product) continue;

                                    int vendorId = std::stoi(std::string(vendor), nullptr, 16);
                                    int productId = std::stoi(std::string(product), nullptr, 16);
                                    
                                    std::string deviceName;
                                    switch(vendorId)
                                    {
                                        case 0x03EB:
                                        {
                                            // Atmel
                                            if(productId == 0x6124)
                                            {
                                                // SAM-BA
                                                deviceName = "Atmel SAM-BA";
                                                break;
                                            }

                                            break;
                                        }
                                        case 0x2341:
                                        {
                                            // Arduino
                                            deviceName = "Caterina";
                                            break;
                                        }
                                        case 0x1B4F:
                                        {
                                            // Sparkfun
                                            deviceName = "Caterina";
                                            break;
                                        }
                                        case 0x239A:
                                        {
                                            // Adafruit
                                            deviceName = "Caterina";
                                            break;
                                        }
                                        case 0x16C0:
                                        {
                                            switch(productId)
                                            {
                                                case 0x0483:
                                                {
                                                    // Arduino ISP
                                                    deviceName = "AVRISP";
                                                    break;
                                                }
                                                case 0x05DC:
                                                {
                                                    // AVR USBAsp
                                                    deviceName = "USBAsp";
                                                    break;
                                                }
                                            }

                                            break;
                                        }
                                        case 0x1781:
                                        { 
                                            if(productId == 0x0C9F)
                                            {
                                                // AVR Pocket ISP
                                                deviceName = "USB Tiny";
                                                break;
                                            }

                                            break;
                                        }
                                    }

                                    if(deviceName != "")
                                    {
                                        std::stringstream message;
                                        message << deviceName << " device " << ((action == "add") ? "connected":"disconnected") << " ";
                                        message << std::string(manufacturerName) + " " + std::string(productName) + " (";
                                        message << std::setfill('0') << std::setw(4) << std::hex << vendorId;
                                        message << ":";
                                        message << std::setw(4) << productId;
                                        message << ")";
                                        if(action == "add") message << " @ " << std::string(devNode);
                                        consoleTextView_->Print(message.str(), ConsoleTextView::MessageType::BOOTLOADER);
                                    }
                                }
                            }
                        }

                        udev_device_unref(device);
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

} // namespace qmk