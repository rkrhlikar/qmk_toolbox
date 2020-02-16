#include "main_window.hpp"

#include <gtkmm/builder.h>

#include "local_file_box.hpp"

namespace qmk
{

    MainWindow::MainWindow(BaseObjectType* object, const Glib::RefPtr<Gtk::Builder>& builder) : Gtk::Window(object),
                                                                                                localFileBox_(nullptr)
    {
        builder->get_widget_derived("box4", localFileBox_);
    }

    MainWindow::~MainWindow() {}

} // namespace qmk