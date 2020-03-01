#include "mcu_list_combo_box.hpp"

#include <fstream>
#include <string>

#include <gtkmm/builder.h>

namespace qmk
{

    McuListComboBox::McuListComboBox(BaseObjectType* object, const Glib::RefPtr<Gtk::Builder>& builder)
                                                                                        : Gtk::ComboBoxText(object)
    {
        std::ifstream mcuList("mcu-list.txt");
        if(!mcuList.is_open()) throw std::runtime_error("Failed to load microcontrollers list");

        std::string entry;
        while(std::getline(mcuList, entry))
        {
            if(entry == "") continue;

            prepend(entry);
        }

        set_active(0);
    }

    McuListComboBox::~McuListComboBox() {}

    void McuListComboBox::SetActiveTextEntry(const std::string& entry)
    {
        auto treeModel = get_model();
        size_t childrenCount = treeModel->children().size();
        for(size_t i = 0; i < childrenCount; i++)
        {
            Glib::ustring value;
            treeModel->children()[i].get_value(0, value);

            if(value == entry)
            {
                set_active(i);
                return;
            }
        }

        throw std::runtime_error("No such text entry exists");
    }

    std::string McuListComboBox::GetActiveTextEntry()
    {
        Glib::ustring value;
        get_active()->get_value(0, value);
        return value;
    }

} // namespace qmk