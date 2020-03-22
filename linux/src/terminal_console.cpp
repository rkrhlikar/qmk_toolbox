
#include "terminal_console.hpp"

#include <iostream>

namespace qmk
{

    TerminalConsole::TerminalConsole() : IConsole(), stop_(false)
    {
        printingThread_ = std::thread([this]{
            // Printing thread
            while(true)
            {
                std::unique_lock<std::mutex> queueLock(messageQueueLock_);
                messageConditionVar_.wait(queueLock, [this]() { return (stop_ || !messageQueue_.empty()); });

                while(!messageQueue_.empty())
                {
                    Message& message = messageQueue_.front();
                    std::string& text = message.text;

                    if(text.back() == '\n') text.erase(text.end() - 1);

                    switch(message.level)
                    {
                        case 0:
                        {
                            switch(message.type)
                            {
                                case MessageType::INFO:
                                    text = "*** " + text;
                                    break;
                                case MessageType::COMMAND:
                                    text = ">>> " + text;
                                    break;
                                case MessageType::BOOTLOADER:
                                    // Yellow text
                                    text = "\033[33m*** " + text + "\033[0m";
                                    break;
                                case MessageType::ERROR:
                                    // Red text
                                    text = "\033[31m  ! " + text + "\033[0m";
                                    break;
                                case MessageType::HID:
                                    // Cyan text
                                    text = "\033[36m*** " + text + "\033[0m";
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
                                    text = "        " + text;
                                    break;
                                case MessageType::COMMAND:
                                    text = "        " + text;
                                    break;
                                case MessageType::BOOTLOADER:
                                    // color = "yellow";
                                    text = "\033[33m        " + text + "\033[0m";
                                    break;
                                case MessageType::ERROR:
                                    // color = "dark_red";
                                    text = "\031[33m        " + text + "\033[0m";
                                    break;
                                case MessageType::HID:
                                    /// color = "sky_blue";
                                    text = "\033[34m        " + text + "\033[0m";
                                    break;
                            }

                            break;
                        }
                    }

                    std::cout << text << std::endl;
                    
                    messageQueue_.pop();
                }

                if(stop_) break;
            }
        });
    }

    TerminalConsole::~TerminalConsole()
    {
        stop_ = true;
        messageConditionVar_.notify_all();
        printingThread_.join();
    }

    void TerminalConsole::Print(std::string text, MessageType msgType)
    {
        {
            std::lock_guard<std::mutex> queueLock(messageQueueLock_);
            messageQueue_.emplace(Message({text, msgType, 0}));
        }

        messageConditionVar_.notify_all();
    }

    void TerminalConsole::PrintResponse(std::string text, MessageType msgType)
    {
        {
            std::lock_guard<std::mutex> queueLock(messageQueueLock_);
            messageQueue_.emplace(Message({text, msgType, 1}));
        }

        messageConditionVar_.notify_all();
    }

} // namespace qmk