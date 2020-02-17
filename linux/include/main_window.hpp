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
    class RemoteFileGrid;

    class MainWindow : public Gtk::Window
    {
    public:
        MainWindow(BaseObjectType* object, const Glib::RefPtr<Gtk::Builder>& builder);
        virtual ~MainWindow();

    protected:
        bool OnExit_(GdkEventAny*);

        LocalFileBox* localFileBox_;
        RemoteFileGrid* remoteFileGrid_;

        std::unique_ptr<UserConfiguration> userConfig_;
    };

} // namespace qmk