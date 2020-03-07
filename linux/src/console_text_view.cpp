#include "console_text_view.hpp"

#include <string>

#include <gtkmm/builder.h>

namespace qmk
{

    ConsoleTextView::ConsoleTextView(BaseObjectType* object, const Glib::RefPtr<Gtk::Builder>& builder)
                                                                                        : Gtk::TextView(object)
    {
        Glib::RefPtr<Gtk::TextBuffer> buffer = get_buffer();
        buffer->create_tag("white")->property_foreground() = "#ffffff";
        buffer->create_tag("yellow")->property_foreground() = "#ffff00";
        buffer->create_tag("red")->property_foreground() = "#ff0000";
        buffer->create_tag("light_sky_blue")->property_foreground() = "#87cefa";
        buffer->create_tag("light_gray")->property_foreground() = "#d3d3d3";
        buffer->create_tag("dark_red")->property_foreground() = "#8b0000";
        buffer->create_tag("sky_blue")->property_foreground() = "#87ceeb";

        messageNotificationDispatcher_.connect(sigc::mem_fun(*this, &ConsoleTextView::OnMessageNotification_));
    }

    ConsoleTextView::~ConsoleTextView() {}

    void ConsoleTextView::Print(std::string text, MessageType msgType)
    {
        {
            std::lock_guard<std::mutex> queueLock(messageQueueLock_);
            messageQueue_.emplace(Message({text, msgType, 0}));
        }

        messageNotificationDispatcher_.emit();
    }

    void ConsoleTextView::PrintResponse(std::string text, MessageType msgType)
    {
        {
            std::lock_guard<std::mutex> queueLock(messageQueueLock_);
            messageQueue_.emplace(Message({text, msgType, 1}));
        }

        messageNotificationDispatcher_.emit();
    }

    void ConsoleTextView::OnMessageNotification_()
    {
        std::lock_guard<std::mutex> queueLock(messageQueueLock_);

        while(!messageQueue_.empty())
        {
            Message& message = messageQueue_.front();
            std::string& text = message.text;

            if(text.back() == '\n') text.erase(text.end() - 1);

            Glib::ustring color = "white";

            switch(message.level)
            {
                case 0:
                {
                    switch(message.type)
                    {
                        case MessageType::INFO:
                            color = "white";
                            text = "*** " + text + "\n";
                            break;
                        case MessageType::COMMAND:
                            color = "white";
                            text = ">>> " + text + "\n";
                            break;
                        case MessageType::BOOTLOADER:
                            color = "yellow";
                            text = "*** " + text + "\n";
                            break;
                        case MessageType::ERROR:
                            color = "red";
                            text = "  ! " + text + "\n";
                            break;
                        case MessageType::HID:
                            color = "light_sky_blue";
                            text = "*** " + text + "\n";
                            break;
                    }

                    break;
                }
                case 1:
                {
                    // Add indentation to line breaks in text
                    size_t newline = text.find('\n', 0);
                    while(newline < std::string::npos)
                    {
                        text.insert(++newline, "        ");
                        newline = text.find('\n', newline);
                    }

                    switch(message.type)
                    {
                        case MessageType::INFO:
                            color = "light_gray";
                            text = "        " + text + "\n";
                            break;
                        case MessageType::COMMAND:
                            color = "light_gray";
                            text = "        " + text + "\n";
                            break;
                        case MessageType::BOOTLOADER:
                            color = "yellow";
                            text = "        " + text + "\n";
                            break;
                        case MessageType::ERROR:
                            color = "dark_red";
                            text = "        " + text + "\n";
                            break;
                        case MessageType::HID:
                            color = "sky_blue";
                            text = "        " + text + "\n";
                            break;
                    }

                    break;
                }
            }

            Glib::RefPtr<Gtk::TextBuffer> buffer = get_buffer();
            buffer->insert_with_tag(buffer->end(), text, color);
            
            messageQueue_.pop();
        }
    }

} // namespace qmk