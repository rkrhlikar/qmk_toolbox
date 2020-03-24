#include <iostream>

#include <gtkmm/application.h>
#include <gtkmm/builder.h>

#include "device_handler.hpp"
#include "main_window.hpp"
#include "terminal_console.hpp"

int main(int argc, char** argv)
{
    if(argc > 1)
    {
        // Received terminal command
        std::string action(argv[1]);

        qmk::TerminalConsole console;

        if(action == "help")
        {
            console.Print("QMK Toolbox (https://qmk.fm/toolbox)", qmk::TerminalConsole::MessageType::INFO);
            console.PrintResponse("Supported bootloaders:", qmk::TerminalConsole::MessageType::INFO);
            console.PrintResponse(" - Atmel/LUFA/QMK DFU via dfu-programmer (http://dfu-programmer.github.io/)", qmk::TerminalConsole::MessageType::INFO);
            console.PrintResponse(" - Caterina (Arduino, Pro Micro) via avrdude (http://nongnu.org/avrdude/)", qmk::TerminalConsole::MessageType::INFO);
            console.PrintResponse(" - Halfkay (Teensy, Ergodox EZ) via Teensy Loader (https://www.pjrc.com/teensy/loader_cli.html)", qmk::TerminalConsole::MessageType::INFO);
            console.PrintResponse(" - ARM DFU (STM32, Kiibohd) via dfu-util (http://dfu-util.sourceforge.net/)", qmk::TerminalConsole::MessageType::INFO);
            console.PrintResponse(" - Atmel SAM-BA (Massdrop) via Massdrop Loader (https://github.com/massdrop/mdloader)", qmk::TerminalConsole::MessageType::INFO);
            console.PrintResponse(" - BootloadHID (Atmel, PS2AVRGB) via bootloadHID (https://www.obdev.at/products/vusb/bootloadhid.html)", qmk::TerminalConsole::MessageType::INFO);
            console.PrintResponse("Supported ISP flashers:", qmk::TerminalConsole::MessageType::INFO);
            console.PrintResponse(" - USBTiny (AVR Pocket)", qmk::TerminalConsole::MessageType::INFO);
            console.PrintResponse(" - AVRISP (Arduino ISP)", qmk::TerminalConsole::MessageType::INFO);
            console.PrintResponse(" - USBasp (AVR ISP)", qmk::TerminalConsole::MessageType::INFO);
            console.PrintResponse(" - USBasp (AVR ISP)", qmk::TerminalConsole::MessageType::INFO);
            console.PrintResponse("", qmk::TerminalConsole::MessageType::INFO);
            console.PrintResponse("Usage: qmk_toolbox <command> [<mcu> <filepath>]", qmk::TerminalConsole::MessageType::INFO);
            console.PrintResponse("", qmk::TerminalConsole::MessageType::INFO);
            console.PrintResponse("Commands:", qmk::TerminalConsole::MessageType::INFO);
            console.PrintResponse("  list\t\tList all currently connected devices", qmk::TerminalConsole::MessageType::INFO);
            console.PrintResponse("  flash\t\tFlash connected devices", qmk::TerminalConsole::MessageType::INFO);
            console.PrintResponse("    Args:", qmk::TerminalConsole::MessageType::INFO);
            console.PrintResponse("      mcu\ttarget", qmk::TerminalConsole::MessageType::INFO);
            console.PrintResponse("      filepath\tpath to firmware file", qmk::TerminalConsole::MessageType::INFO);
            console.PrintResponse("  help\t\tPrint help with available commands", qmk::TerminalConsole::MessageType::INFO);
        }
        else if(action == "list")
        {
            qmk::DeviceHandler deviceHandler;
            deviceHandler.Initialize(&console);
        }
        else if(action == "flash")
        {
            if(argc != 4)
            {
                std::cout << "Invalid argument count!" << std::endl;
                std::cout << "Run command 'help' to see list of possible commands and corresponding arguments." << std::endl;
                return -1;
            }

            qmk::DeviceHandler deviceHandler;
            deviceHandler.Initialize(&console);
            deviceHandler.FlashConnectedDevices(std::string(argv[2]), std::string(argv[3]));
        }
        else
        {
            std::cout << "Unknown command '" + action + "'!" << std::endl;
            std::cout << "Run command 'help' to see list of possible commands." << std::endl;
            return -1;
        }
        
        return 0;
    }
    else
    {
        // Run GUI
        auto app = Gtk::Application::create(argc, argv, "fm.qmk.toolkit");
        auto builder = Gtk::Builder::create_from_file("assets/main.glade", "main");

        qmk::MainWindow* pMainWindow;
        builder->get_widget_derived("main", pMainWindow);

        if(pMainWindow)
        {
            return app->run(*pMainWindow); 
        }
    }

    return -1;
}