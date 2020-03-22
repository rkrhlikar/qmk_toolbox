#pragma once

#include <gtkmm/box.h>

#include "device_handler.hpp"

namespace Gtk
{
    class Builder;
    class Button;
} // namespace Gtk

namespace qmk
{
    
    class IConsole;
    class LocalFileBox;
    class McuListComboBox;

    class FlashingBox : public Gtk::Box
    {
    public:
        FlashingBox(BaseObjectType* object, const Glib::RefPtr<Gtk::Builder>& builder);
        virtual ~FlashingBox();

        void Initialize(IConsole* console,
                        LocalFileBox* localFileBox,
                        McuListComboBox* mcuListComboBox);

    protected:
        void OnButtonFlash_();

        Gtk::Button* flashButton_;

        LocalFileBox* localFileBox_;
        McuListComboBox* mcuListComboBox_;

        DeviceHandler deviceHandler_;
    };

} // namespace qmk