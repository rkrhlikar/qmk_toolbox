#pragma once

#include <gtkmm/window.h>

namespace Gtk
{
    class Builder;
} // namespace Gtk

namespace qmk
{

    class MainWindow : public Gtk::Window
    {
    public:
        MainWindow(BaseObjectType* object, const Glib::RefPtr<Gtk::Builder>& builder);
        virtual ~MainWindow();
    };

} // namespace qmk