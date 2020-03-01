#pragma once

#include <gtkmm/textview.h>

namespace Gtk
{
    class Builder;
} // namespace Gtk

namespace qmk
{

    class ConsoleTextView : public Gtk::TextView
    {
    public:
        enum class MessageType
        {
            BOOTLOADER,
            HID,
            COMMAND,
            INFO,
            ERROR
        };

        ConsoleTextView(BaseObjectType* object, const Glib::RefPtr<Gtk::Builder>& builder);
        virtual ~ConsoleTextView();

        void Print(std::string text, MessageType msgType);
        void PrintResponse(std::string text, MessageType msgType);
    };

} // namespace qmk