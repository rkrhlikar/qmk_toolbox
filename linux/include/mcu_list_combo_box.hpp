#pragma once

#include <gtkmm/comboboxtext.h>

namespace Gtk
{
    class Builder;
} // namespace Gtk

namespace qmk
{

    class McuListComboBox : public Gtk::ComboBoxText
    {
    public:
        McuListComboBox(BaseObjectType* object, const Glib::RefPtr<Gtk::Builder>& builder);
        virtual ~McuListComboBox();

        void SetActiveTextEntry(const std::string& entry);
        std::string GetActiveTextEntry();
    };

} // namespace qmk