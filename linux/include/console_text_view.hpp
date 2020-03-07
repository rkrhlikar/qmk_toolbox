#pragma once

#include <mutex>
#include <queue>
#include <string>

#include <glibmm/dispatcher.h>
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

    private:
        Glib::Dispatcher messageNotificationDispatcher_;
        void OnMessageNotification_();

        struct Message
        {
            std::string text;
            MessageType type;
            size_t level;
        };

        std::mutex messageQueueLock_;
        std::queue<Message> messageQueue_;
    };

} // namespace qmk