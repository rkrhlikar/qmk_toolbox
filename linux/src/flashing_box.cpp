#include "flashing_box.hpp"

#include <string>

#include <gtkmm/builder.h>
#include <gtkmm/button.h>

#include "iconsole.hpp"
#include "local_file_box.hpp"
#include "mcu_list_combo_box.hpp"

namespace qmk
{

    FlashingBox::FlashingBox(BaseObjectType* object, const Glib::RefPtr<Gtk::Builder>& builder)
                                                                                        : Gtk::Box(object),
                                                                                          flashButton_(nullptr)
    {
        builder->get_widget("flash", flashButton_);

        if(flashButton_)
        {
            flashButton_->signal_clicked().connect(sigc::mem_fun(*this, &FlashingBox::OnButtonFlash_));
        }
    }

    FlashingBox::~FlashingBox() {}

    void FlashingBox::Initialize(IConsole* console, LocalFileBox* localFileBox, McuListComboBox* mcuListComboBox)
    {
        deviceHandler_.Initialize(console);

        localFileBox_ = localFileBox;
        mcuListComboBox_ = mcuListComboBox;
    }

    void FlashingBox::OnButtonFlash_()
    {
        std::string microcontroller = mcuListComboBox_->GetActiveTextEntry();
        std::string filePath = localFileBox_->GetActiveEntry();

        deviceHandler_.FlashConnectedDevices(microcontroller, filePath);
    }

} // namespace qmk