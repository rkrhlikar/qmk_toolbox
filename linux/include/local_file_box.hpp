#pragma once

#include <gtkmm/box.h>

namespace Gtk
{
    class Builder;
    class Button;
    class ComboBox;
} // namespace Gtk

namespace qmk
{

    class LocalFileBox : public Gtk::Box
    {
    public:
        LocalFileBox(BaseObjectType* object, const Glib::RefPtr<Gtk::Builder>& builder);
        virtual ~LocalFileBox();

    protected:
        void OnButtonOpen_();

        Gtk::Button* openButton_;
        Gtk::ComboBox* localFileComboBox_;
    };

} // namespace qmk