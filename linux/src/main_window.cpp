#include "main_window.hpp"

#include <cstdlib>
#include <experimental/filesystem>

#include <gtkmm/builder.h>

#include "local_file_box.hpp"

namespace qmk
{

    MainWindow::MainWindow(BaseObjectType *object, const Glib::RefPtr<Gtk::Builder> &builder) : Gtk::Window(object),
                                                                                                localFileBox_(nullptr)
    {
        builder->get_widget_derived("box4", localFileBox_);
    }

    MainWindow::~MainWindow() {}

    void MainWindow::Initialize()
    {
        namespace fs = std::experimental::filesystem;

        std::string confDir = std::string(std::getenv("HOME")) + "/.config/qmk_toolkit";
        if(!fs::exists(confDir))
        {
            // Configuration directory does not exist
            // Create it
            fs::create_directory(confDir);
        }

        userConfig_ = UserConfiguration(confDir);

        localFileBox_->Initialize(&userConfig_);
    }

} // namespace qmk