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
                                        device.deviceName = "DFU";
                                        break;
                                    }
                                    case 0x16C0:
                                    {
                                        switch(productId)
                                        {
                                            case 0x05DF:
                                            {
                                                // Objective Development
                                                device.deviceName = "BootloadHID";
                                                break;
                                            }
                                            case 0x0478:
                                            {
                                                // PJRC
                                                device.deviceName = "Halfkay";
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
                                                device.deviceName = "STM32";
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
                                                device.deviceName = "Kiibohd";
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
                                                device.deviceName = "Atmel SAM-BA";
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
                                        device.deviceName = "Caterina";
                                        break;
                                    }
                                    case 0x1B4F:
                                    {
                                        // Sparkfun
                                        device.deviceName = "Caterina";
                                        break;
                                    }
                                    case 0x239A:
                                    {
                                        // Adafruit
                                        device.deviceName = "Caterina";
                                        break;
                                    }
                                    case 0x16C0:
                                    {
                                        switch(productId)
                                        {
                                            case 0x0483:
                                            {
                                                // Arduino ISP
                                                device.deviceName = "AVRISP";
                                                break;
                                            }
                                            case 0x05DC:
                                            {
                                                // AVR USBAsp
                                                device.deviceName = "USBAsp";
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
                                                device.deviceName = "USB Tiny";
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
                        }
                        else if(action == "remove")
                        {
                            continue;
                        }
                        else
                        {
                            continue;
                        }
                        

                        std::stringstream message;
                        message << device.deviceName << " device " << ((action == "add") ? "connected":"disconnected") << " ";
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

} // namespace qmk