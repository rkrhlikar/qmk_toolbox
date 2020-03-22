#include "user_configuration.hpp"

#include <experimental/filesystem>
#include <fstream>

#include "local_file_box.hpp"
#include "mcu_list_combo_box.hpp"
#include "toml11/toml.hpp"

namespace qmk
{

    UserConfiguration::UserConfiguration(const std::string& configDir, LocalFileBox* localFileBox, McuListComboBox* mcuListComboBox)
    {
        namespace fs = std::experimental::filesystem;
        
        localFileBox_ = localFileBox;
        mcuListComboBox_ = mcuListComboBox;

        filePath_ = configDir + "/user.config";

        if(fs::exists(filePath_))
        {
            const toml::value config = toml::parse(filePath_);

            const toml::table& hexFileSettings = toml::find<toml::table>(config, "hex_files");

            std::vector<std::string> localFilesList = toml::get<std::vector<std::string>>(hexFileSettings.at("list"));
            for(std::string& entry : localFilesList)
            {
                localFileBox_->AppendEntry(entry);
            }

            std::string activeEntry = toml::get<std::string>(hexFileSettings.at("active"));
            localFileBox_->AddActiveEntry(activeEntry);

            const toml::table& targetSettings = toml::find<toml::table>(config, "target");
            std::string mcu = toml::get<std::string>(targetSettings.at("mcu"));
            mcuListComboBox_->SetActiveTextEntry(mcu);
        }
    }

    UserConfiguration::~UserConfiguration()
    {
        toml::value config;

        std::vector<std::string> hexFileList = localFileBox_->GetLocalFilesList();

        if(!hexFileList.empty())
        {

            toml::array listEntries;
            for(std::vector<std::string>::const_iterator listIt = hexFileList.begin() + 1;
                listIt != hexFileList.end(); 
                listIt++)
            {
                listEntries.push_back(*listIt);
            }

            toml::table hexFiles {{"active", hexFileList[0]}, {"list", std::move(listEntries)}};
            config["hex_files"] = std::move(hexFiles);
        }

        toml::table targetSettings {{"mcu", mcuListComboBox_->GetActiveTextEntry()}};
        config["target"] = std::move(targetSettings);

        std::ofstream configFile(filePath_);        
        configFile << config;
    }

} // namespace qmk