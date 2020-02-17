#include "main_window.hpp"

#include <cstdlib>
#include <experimental/filesystem>

#include <gtkmm/builder.h>

#include "local_file_box.hpp"
#include "remote_file_grid.hpp"

namespace qmk
{

    MainWindow::MainWindow(BaseObjectType *object, const Glib::RefPtr<Gtk::Builder> &builder) : Gtk::Window(object),
                                                                                                localFileBox_(nullptr),
                                                                                                remoteFileGrid_(nullptr)
    {
        this->signal_delete_event().connect(sigc::mem_fun(*this, &MainWindow::OnExit_));

        builder->get_widget_derived("box4", localFileBox_);
        builder->get_widget_derived("grid2", remoteFileGrid_);

        // Application initialization
        namespace fs = std::experimental::filesystem;

        std::string confDir = std::string(std::getenv("HOME")) + "/.config/qmk_toolkit";
        if(!fs::exists(confDir))
        {
            // Configuration directory does not exist
            // Create it
            fs::create_directory(confDir);
        }

        userConfig_ = std::make_unique<UserConfiguration>(confDir);

        localFileBox_->Initialize(userConfig_.get());
        remoteFileGrid_->Initialize(localFileBox_);
    }

    MainWindow::~MainWindow() {}

    bool MainWindow::OnExit_(GdkEventAny*)
    {
        userConfig_.reset(nullptr);
        return false;
    }

} // namespace qmk