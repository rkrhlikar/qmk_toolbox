#pragma once

#include <gtkmm/box.h>

namespace Gtk
{
    class Builder;
    class Button;
    class ComboBoxText;
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
        Gtk::ComboBoxText* localFileComboBox_;
    };

} // namespace qmk