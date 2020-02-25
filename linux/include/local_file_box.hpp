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

        void AddActiveEntry(const std::string& filePath);
        void AppendEntry(const std::string& filePath);
        std::vector<std::string> GetLocalFilesList();

    protected:
        void OnButtonOpen_();
        void OnChangedFileSelection_();
        int FindDuplicateEntry_(const std::string& filePath);

        Gtk::Button* openButton_;
        Gtk::ComboBoxText* localFileComboBox_;
    };

} // namespace qmk