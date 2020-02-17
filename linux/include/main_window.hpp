#pragma once

#include <gtkmm/window.h>

#include "user_configuration.hpp"

namespace Gtk
{
    class Builder;
} // namespace Gtk

namespace qmk
{

    class LocalFileBox;

    class MainWindow : public Gtk::Window
    {
    public:
        MainWindow(BaseObjectType* object, const Glib::RefPtr<Gtk::Builder>& builder);
        virtual ~MainWindow();

        void Initialize();

    protected:
        LocalFileBox* localFileBox_;

        UserConfiguration userConfig_;
    };

} // namespace qmk