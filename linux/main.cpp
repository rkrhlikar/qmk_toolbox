#include <gtkmm.h>

int main(int argc, char** argv)
{
    auto app = Gtk::Application::create(argc, argv, "fm.qmk.toolkit");
    auto builder = Gtk::Builder::create_from_file("main.glade", "main");

    Gtk::Window* pMainWindow;
    builder->get_widget("main", pMainWindow);

    return app->run(*pMainWindow);
}