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
        this->signal_delete_event().connect(sigc::mem_fun(*this, &MainWindow::OnExit_));

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

        userConfig_ = std::make_unique<UserConfiguration>(confDir);

        localFileBox_->Initialize(userConfig_.get());
    }

    bool MainWindow::OnExit_(GdkEventAny*)
    {
        userConfig_.reset(nullptr);
        return false;
    }

} // namespace qmk