#include "main_window.hpp"

#include <gtkmm/builder.h>

namespace qmk
{

    MainWindow::MainWindow(BaseObjectType* object, const Glib::RefPtr<Gtk::Builder>& builder) : Gtk::Window(object)
    {

    }

    MainWindow::~MainWindow() {}

} // namespace qmk