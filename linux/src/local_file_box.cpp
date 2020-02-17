#include "local_file_box.hpp"

#include <string>

#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/filechooserdialog.h>

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
        Gtk::FileChooserDialog dialog("Please choose a file", Gtk::FILE_CHOOSER_ACTION_OPEN);
        dialog.set_transient_for(dynamic_cast<Gtk::Window&>(*(get_toplevel())));

        dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
        dialog.add_button("_Open", Gtk::RESPONSE_OK);

        auto filterBoth = Gtk::FileFilter::create();
        filterBoth->set_name("Intel Hex and Binary");
        filterBoth->add_pattern("*.hex");
        filterBoth->add_pattern("*.bin");
        dialog.add_filter(filterBoth);

        auto filterHex = Gtk::FileFilter::create();
        filterHex->set_name("Intel Hex");
        filterHex->add_pattern("*.hex");
        dialog.add_filter(filterHex);

        auto filterBin = Gtk::FileFilter::create();
        filterBin->set_name("Binary");
        filterBin->add_pattern("*.bin");
        dialog.add_filter(filterBin);

        switch (dialog.run())
        {
            case(Gtk::RESPONSE_OK):
            {
                std::string filename = dialog.get_filename();
                localFileComboBox_->prepend(filename);
                localFileComboBox_->set_active(0);
                break;
            }
            case(Gtk::RESPONSE_CANCEL):
                break;        
            default:
                break;
        }
    }

    void LocalFileBox::Initialize(UserConfiguration* userConfig)
    {
        userConfig_ = userConfig;
        const std::vector<std::string>& localFileList = userConfig_->GetLocalFileList();

        for(const std::string& filename : localFileList)
        {
            localFileComboBox_->append(filename);
        }
        
        localFileComboBox_->set_active(0);
    }

} // namespace qmk