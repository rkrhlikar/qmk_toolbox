#include "remote_file_grid.hpp"

#include <cstdlib>
#include <sstream>
#include <fstream>
#include <vector>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/comboboxtext.h>

#include "local_file_box.hpp"

namespace qmk
{

    static std::vector<std::string> GetKeyboardsList(std::string listString)
    {
        std::vector<std::string> output;

        size_t start;
        size_t end = 0;

        while((start = listString.find_first_not_of(',', end)) != std::string::npos)
        {
            end = listString.find(',', start);
            output.push_back(listString.substr(start + 1, end - start - 2));
        }

        return output;
    }
    
    RemoteFileGrid::RemoteFileGrid(BaseObjectType* object, const Glib::RefPtr<Gtk::Builder>& builder)
                                                                                        : Gtk::Grid(object),
                                                                                          loadButton_(nullptr),
                                                                                          keyboardComboBox_(nullptr),
                                                                                          keymapComboBox_(nullptr)
    {
        builder->get_widget("keyboard", keyboardComboBox_);
        builder->get_widget("keymap", keymapComboBox_);
        builder->get_widget("load", loadButton_);

        if(loadButton_)
        {
            loadButton_->signal_clicked().connect(sigc::mem_fun(*this, &RemoteFileGrid::OnButtonLoad_));
        }

        if(keyboardComboBox_)
        {
            curlpp::Cleanup curlCleanup;

            curlpp::options::Url qmkApiUrl("http://api.qmk.fm/v1/keyboards");
            curlpp::Easy apiReq;
            apiReq.setOpt(qmkApiUrl);

            std::ostringstream resStream;
            resStream << apiReq;

            std::string rawKeyboardList = resStream.str();

            std::vector<std::string> keyboardsList = GetKeyboardsList(rawKeyboardList.substr(1, rawKeyboardList.size() - 2));
            for(const std::string& keyboard : keyboardsList)
            {
                keyboardComboBox_->append(keyboard);
            }
        }

        if(keymapComboBox_)
        {
            keymapComboBox_->append("default");            
            keymapComboBox_->set_active(0);            
        }
    }

    RemoteFileGrid::~RemoteFileGrid() {}

    void RemoteFileGrid::Initialize(LocalFileBox* localFileBox)
    {
        localFileBox_ = localFileBox;
    }

    void RemoteFileGrid::OnButtonLoad_()
    {
        std::string keyboard = keyboardComboBox_->get_active_text();
        if(keyboard == "") return;

        size_t index = 0;
        while((index = keyboard.find('/', index)) != std::string::npos)
        {
            keyboard.replace(index, 1, "_");
            index++;
        }

        keyboard += "_default.hex";

        curlpp::Cleanup curlCleanup;

        curlpp::options::Url qmkFwUrl("https://qmk.fm/compiled/" + keyboard);
        curlpp::Easy qmkFwReq;
        qmkFwReq.setOpt(qmkFwUrl);

        std::string outputFile = std::string(std::getenv("HOME")) + "/.config/qmk_toolkit/" + keyboard;
        std::ofstream fwFileStream(outputFile);
        fwFileStream << qmkFwReq;
        
        localFileBox_->AddActiveEntry(outputFile);
    }    

} // namespace qmk