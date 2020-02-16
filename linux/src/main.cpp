#include <gtkmm/application.h>
#include <gtkmm/builder.h>

#include "main_window.hpp"

int main(int argc, char** argv)
{
    auto app = Gtk::Application::create(argc, argv, "fm.qmk.toolkit");
    auto builder = Gtk::Builder::create_from_file("main.glade", "main");

    qmk::MainWindow* pMainWindow;
    builder->get_widget_derived("main", pMainWindow);

    return app->run(*pMainWindow);
}