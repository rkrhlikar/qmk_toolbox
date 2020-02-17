#pragma once

#include <gtkmm/grid.h>

namespace Gtk
{
    class Builder;
    class Button;
    class ComboBoxText;
} // namespace Gtk

namespace qmk
{

    class LocalFileBox;

    class RemoteFileGrid : public Gtk::Grid
    {
    public:
        RemoteFileGrid(BaseObjectType* object, const Glib::RefPtr<Gtk::Builder>& builder);
        virtual ~RemoteFileGrid();

        void Initialize(LocalFileBox* localFileBox);

    protected:
        void OnButtonLoad_();

        LocalFileBox* localFileBox_;

        Gtk::ComboBoxText* keyboardComboBox_;
        Gtk::ComboBoxText* keymapComboBox_;
        Gtk::Button* loadButton_;
    };

} // namespace qmk