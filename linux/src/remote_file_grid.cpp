#include "remote_file_grid.hpp"

#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/comboboxtext.h>

namespace qmk
{
    
    RemoteFileGrid::RemoteFileGrid(BaseObjectType* object, const Glib::RefPtr<Gtk::Builder>& builder)
                                                                                        : Gtk::Grid(object),
                                                                                          loadButton_(nullptr),
                                                                                          keyboardComboBox_(nullptr),
                                                                                          keymapComboBox_(nullptr)
    {
        builder->get_widget("keyboard", keyboardComboBox_);
        builder->get_widget("keymap", keymapComboBox_);
        builder->get_widget("load", loadButton_);

        if(loadButton_)
        {
            loadButton_->signal_clicked().connect(sigc::mem_fun(*this, &RemoteFileGrid::OnButtonLoad_));
        }

        if(keyboardComboBox_)
        {

        }

        if(keymapComboBox_)
        {
            keymapComboBox_->append("default");            
            keymapComboBox_->set_active(0);            
        }
    }

    RemoteFileGrid::~RemoteFileGrid() {}

    void RemoteFileGrid::OnButtonLoad_()
    {

    }    

} // namespace qmk