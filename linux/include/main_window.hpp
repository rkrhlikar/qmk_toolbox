#pragma once

#include <gtkmm.h>

namespace qmk
{

    class MainWindow : public Gtk::Window
    {
    public:
        MainWindow(BaseObjectType* object, const Glib::RefPtr<Gtk::Builder>& builder);
        virtual ~MainWindow();
    };

} // namespace qmk