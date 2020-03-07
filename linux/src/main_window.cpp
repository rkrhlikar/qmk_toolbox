#include "main_window.hpp"

#include <cstdlib>
#include <experimental/filesystem>

#include <gtkmm/builder.h>

#include "console_text_view.hpp"
#include "local_file_box.hpp"
#include "mcu_list_combo_box.hpp"
#include "remote_file_grid.hpp"
#include "flashing_box.hpp"

namespace qmk
{

    MainWindow::MainWindow(BaseObjectType *object, const Glib::RefPtr<Gtk::Builder> &builder) : Gtk::Window(object),
                                                                                                localFileBox_(nullptr),
                                                                                                remoteFileGrid_(nullptr),
                                                                                                consoleTextView_(nullptr),
                                                                                                mcuListComboBox_(nullptr),
                                                                                                flashingBox_(nullptr)
    {
        this->signal_delete_event().connect(sigc::mem_fun(*this, &MainWindow::OnExit_));

        builder->get_widget_derived("box4", localFileBox_);
        builder->get_widget_derived("grid2", remoteFileGrid_);
        builder->get_widget_derived("output", consoleTextView_);
        builder->get_widget_derived("microcontroller", mcuListComboBox_);
        builder->get_widget_derived("box7", flashingBox_);

        // Application initialization
        namespace fs = std::experimental::filesystem;

        std::string confDir = std::string(std::getenv("HOME")) + "/.config/qmk_toolkit";
        if(!fs::exists(confDir))
        {
            // Configuration directory does not exist
            // Create it
            fs::create_directory(confDir);
        }

        remoteFileGrid_->Initialize(localFileBox_);

        userConfig_ = std::make_unique<UserConfiguration>(confDir, localFileBox_, mcuListComboBox_);

        consoleTextView_->Print("QMK Toolbox (https://qmk.fm/toolbox)", ConsoleTextView::MessageType::INFO);
        consoleTextView_->PrintResponse("Supported bootloaders:", ConsoleTextView::MessageType::INFO);
        consoleTextView_->PrintResponse(" - Atmel/LUFA/QMK DFU via dfu-programmer (http://dfu-programmer.github.io/)", ConsoleTextView::MessageType::INFO);
        consoleTextView_->PrintResponse(" - Caterina (Arduino, Pro Micro) via avrdude (http://nongnu.org/avrdude/)", ConsoleTextView::MessageType::INFO);
        consoleTextView_->PrintResponse(" - Halfkay (Teensy, Ergodox EZ) via Teensy Loader (https://www.pjrc.com/teensy/loader_cli.html)", ConsoleTextView::MessageType::INFO);
        consoleTextView_->PrintResponse(" - ARM DFU (STM32, Kiibohd) via dfu-util (http://dfu-util.sourceforge.net/)", ConsoleTextView::MessageType::INFO);
        consoleTextView_->PrintResponse(" - Atmel SAM-BA (Massdrop) via Massdrop Loader (https://github.com/massdrop/mdloader)", ConsoleTextView::MessageType::INFO);
        consoleTextView_->PrintResponse(" - BootloadHID (Atmel, PS2AVRGB) via bootloadHID (https://www.obdev.at/products/vusb/bootloadhid.html)", ConsoleTextView::MessageType::INFO);
        consoleTextView_->PrintResponse("Supported ISP flashers:", ConsoleTextView::MessageType::INFO);
        consoleTextView_->PrintResponse(" - USBTiny (AVR Pocket)", ConsoleTextView::MessageType::INFO);
        consoleTextView_->PrintResponse(" - AVRISP (Arduino ISP)", ConsoleTextView::MessageType::INFO);
        consoleTextView_->PrintResponse(" - USBasp (AVR ISP)", ConsoleTextView::MessageType::INFO);

        flashingBox_->Initialize(consoleTextView_, localFileBox_, mcuListComboBox_);
    }

    MainWindow::~MainWindow() {}

    bool MainWindow::OnExit_(GdkEventAny*)
    {
        userConfig_.reset(nullptr);
        return false;
    }

} // namespace qmk