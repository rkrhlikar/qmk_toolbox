#include "device_handler.hpp"

#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>

#include <sys/time.h>
#include <libudev.h>
#include <libusb-1.0/libusb.h>

#include "console_text_view.hpp"

namespace qmk
{

    struct Device
    {
        uint16_t vendorId;
        uint16_t productId;
        std::string manufacturerName;
        std::string productName;
        std::string port = "";
    };

    static void GetDevicePort(Device& device)
    {   
        std::stringstream vendorIdStringStream;
        vendorIdStringStream << std::setfill('0') << std::setw(4) << std::hex << device.vendorId;
        std::stringstream productIdStringStream;
        productIdStringStream << std::setfill('0') << std::setw(4) << std::hex << device.productId;

        std::string vendorIdString = vendorIdStringStream.str();
        std::string productIdString = productIdStringStream.str();

        struct udev* udevCtx = udev_new();

        struct udev_enumerate* enumerate = udev_enumerate_new(udevCtx);

        udev_enumerate_add_match_subsystem(enumerate, "tty");
        udev_enumerate_scan_devices(enumerate);

        struct udev_list_entry* devices = udev_enumerate_get_list_entry(enumerate);
        struct udev_list_entry* entry;

        udev_list_entry_foreach(entry, devices)
        {
            const char* path = udev_list_entry_get_name(entry);
            struct udev_device* udevDevice = udev_device_new_from_syspath(udevCtx, path);

            if(udevDevice)
            {
                const char* devNode = udev_device_get_devnode(udevDevice);
                if(devNode)
                {
                    const char* vendor = udev_device_get_property_value(udevDevice, "ID_VENDOR_ID");
                    if (! vendor)
                        vendor = "0000";

                    const char* product = udev_device_get_property_value(udevDevice, "ID_MODEL_ID");
                    if (! product)
                        product = "0000";

                    if(std::string(vendor) == vendorIdString && std::string(product) == productIdString)
                    {
                        device.port = devNode;
                    }
                }

                udev_device_unref(udevDevice);
            }
        }

        udev_enumerate_unref(enumerate);

        udev_unref(udevCtx);
    }

    static int HotplugEventTrampoline(struct libusb_context *context,
                                      struct libusb_device *device,
                                      libusb_hotplug_event event,
                                      void *deviceHandler)
    {
        bool connected = false;
        
        struct libusb_device_descriptor descriptor;
        libusb_get_device_descriptor(device, &descriptor);

        std::string manufacturerName, productName;

        if(event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED)
        {
            connected = true;
            libusb_device_handle* deviceHandle;
            if(libusb_open(device, &deviceHandle) == LIBUSB_SUCCESS)
            {
                unsigned char data[256];
                libusb_get_string_descriptor_ascii(deviceHandle, descriptor.iManufacturer, data, 256);
                manufacturerName = std::string(reinterpret_cast<char*>(data));
                libusb_get_string_descriptor_ascii(deviceHandle, descriptor.iProduct, data, 256);
                productName == std::string(reinterpret_cast<char*>(data));

                libusb_close(deviceHandle);
            }
        }

        static_cast<DeviceHandler*>(deviceHandler)->DeviceEventHandler({descriptor.idVendor,
                                                                        descriptor.idProduct,
                                                                        manufacturerName,
                                                                        productName},
                                                                        connected);

        return 0;
    }

    DeviceHandler::DeviceHandler() {}

    void DeviceHandler::Initialize(ConsoleTextView* consoleTextView)
    {
        consoleTextView_ = consoleTextView;

        libusb_init(nullptr);
        
        int rc = libusb_hotplug_register_callback(nullptr, 
                                                  static_cast<libusb_hotplug_event>(
                                                      LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | 
                                                      LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT
                                                  ),
                                                  LIBUSB_HOTPLUG_ENUMERATE,
                                                  LIBUSB_HOTPLUG_MATCH_ANY,
                                                  LIBUSB_HOTPLUG_MATCH_ANY,
                                                  LIBUSB_HOTPLUG_MATCH_ANY,
                                                  HotplugEventTrampoline,
                                                  static_cast<void*>(this),
                                                  &callbackHandle_);

        if (LIBUSB_SUCCESS != rc) {
            libusb_exit(nullptr);
            throw std::runtime_error("Error creating a usb hotplug callback");
        }

        stop_ = false;
        usbThread_ = std::thread([this](){
            struct timeval time;
            time.tv_sec = 0;
            time.tv_usec = 0;

            while(!stop_)
                libusb_handle_events_timeout_completed(nullptr, &time, nullptr);
        });
    }

    DeviceHandler::~DeviceHandler()
    {
        stop_ = true;
        usbThread_.join();
        libusb_hotplug_deregister_callback(nullptr, callbackHandle_);
        libusb_exit(nullptr);
    }

    void DeviceHandler::DeviceEventHandler(Device&& device, bool connected)
    {
        std::string deviceName;
        switch(device.vendorId)
        {
            case 0x03EB:
            {
                // Atmel
                switch(device.productId)
                {
                    case 0x6124:
                    {
                        // SAM-BA
                        deviceName = "Atmel SAM-BA";
                        if(connected) GetDevicePort(device);
                        break;
                    }
                    default:
                    {
                        // Other Atmel devices
                        deviceName = "DFU";
                        break;
                    }
                }
                break;
            }
            case 0x2341:
            {
                // Arduino
                deviceName = "Caterina";
                if(connected) GetDevicePort(device);
                break;
            }
            case 0x1B4F:
            {
                // Sparkfun
                deviceName = "Caterina";
                if(connected) GetDevicePort(device);
                break;
            }
            case 0x239A:
            {
                // Adafruit
                deviceName = "Caterina";
                if(connected) GetDevicePort(device);
                break;
            }
            case 0x16C0:
            {
                switch(device.productId)
                {
                    case 0x0483:
                    {
                        // Arduino ISP
                        deviceName = "AVRISP";
                        if(connected) GetDevicePort(device);
                        break;
                    }
                    case 0x05DC:
                    {
                        // AVR USBAsp
                        deviceName = "USBAsp";
                        if(connected) GetDevicePort(device);
                        break;
                    }
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
                    default:
                    {
                        // Other devices
                        return;
                    }
                }

                break;
            }
            case 0x0483:
            { 
                if(device.productId == 0xDF11)
                {
                    // STM32
                    deviceName = "STM32";
                    break;
                }

                // Other devices
                return;
            }
            case 0x1C11:
            {
                if(device.productId == 0xB007)
                { 
                    // Kiibohd
                    deviceName = "Kiibohd";
                    break;
                }

                // Other devices
                return;
            }
            case 0x1781:
            { 
                if(device.productId == 0x0C9F)
                {
                    // AVR Pocket ISP
                    deviceName = "USB Tiny";
                    if(connected) GetDevicePort(device);
                    break;
                }

                // Other devices
                return;
            }
            default:
            {
                // Other devices
                return;
            }
        }
        
        std::stringstream message;
        message << deviceName << " device " << (connected ? "connected":"disconnected") << " ";
        message << device.manufacturerName + " " + device.productName + " (";
        message << std::setfill('0') << std::setw(4) << std::hex << device.vendorId;
        message << ":";
        message << std::setw(4) << device.productId;
        message << ")";
        consoleTextView_->Print(message.str(), ConsoleTextView::MessageType::BOOTLOADER);
    }

} // namespace qmk