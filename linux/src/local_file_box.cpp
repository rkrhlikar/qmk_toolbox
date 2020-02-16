#include "local_file_box.hpp"

#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/combobox.h>

namespace qmk
{

    LocalFileBox::LocalFileBox(BaseObjectType* object, const Glib::RefPtr<Gtk::Builder>& builder)
                                                                                        : Gtk::Box(object),
                                                                                          openButton_(nullptr),
                                                                                          localFileComboBox_(nullptr)
    {
        builder->get_widget("local_file", localFileComboBox_);
        builder->get_widget("open", openButton_);

        if(openButton_)
        {
            openButton_->signal_clicked().connect(sigc::mem_fun(*this, &LocalFileBox::OnButtonOpen_));
        }
    }

    LocalFileBox::~LocalFileBox() {}

    void LocalFileBox::OnButtonOpen_()
    {

    }

} // namespace qmk