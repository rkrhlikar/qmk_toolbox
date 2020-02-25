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

        if(localFileComboBox_)
        {
            localFileComboBox_->signal_changed().connect(sigc::mem_fun(*this, &LocalFileBox::OnChangedFileSelection_));
        }
    }

    LocalFileBox::~LocalFileBox() {}

    void LocalFileBox::AddActiveEntry(const std::string& filePath)
    {
        int duplicateEntry = FindDuplicateEntry_(filePath);

        if(duplicateEntry >= 0)
        {
            // Entry already exists, activate it
            localFileComboBox_->set_active(duplicateEntry);
            return;
        }

        localFileComboBox_->prepend(filePath);
        localFileComboBox_->set_active(0);
    }
    
    void LocalFileBox::AppendEntry(const std::string& filePath)
    {
        if(FindDuplicateEntry_(filePath) >= 0)
        {
            // Entry already exists, don't add duplicate
            return;
        }

        localFileComboBox_->append(filePath);
    }

    std::vector<std::string> LocalFileBox::GetLocalFilesList()
    {
        std::vector<std::string> hexFileList;
        auto treeModel = localFileComboBox_->get_model();
        for(auto child : treeModel->children())
        {
            Glib::ustring value;
            (*child).get_value(0, value);
            hexFileList.push_back(value);
        }

        return hexFileList;
    }

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
                AddActiveEntry(dialog.get_filename());

                break;
            }
            case(Gtk::RESPONSE_CANCEL):
                break;        
            default:
                break;
        }
    }

    void LocalFileBox::OnChangedFileSelection_()
    {
        int activeIndex = localFileComboBox_->get_active_row_number();
    
        // If the top entry was activated, don't do anything
        if(activeIndex <= 0) return;

        // An old entry was activated, move it on top of list
        Glib::ustring entry = localFileComboBox_->get_active_text();
        localFileComboBox_->remove_text(activeIndex);
        localFileComboBox_->prepend(entry);
        localFileComboBox_->set_active(0);
    }

    int LocalFileBox::FindDuplicateEntry_(const std::string& filePath)
    {
        std::vector<std::string> filesList = GetLocalFilesList();

        for(int i = 0; i < filesList.size(); i++)
        {
            if(filePath == filesList[i]) return i;
        }

        return -1;
    }

} // namespace qmk