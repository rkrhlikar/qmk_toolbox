#include "user_configuration.hpp"

#include <experimental/filesystem>

#include <libxml++/libxml++.h>

#include "local_file_box.hpp"

namespace qmk
{

    UserConfiguration::UserConfiguration(const std::string& configDir, LocalFileBox* localFileBox)
    {
        namespace fs = std::experimental::filesystem;
        
        localFileBox_ = localFileBox;

        filePath_ = configDir + "/user.config";

        std::string activeLocalFile = "";

        if(fs::exists(filePath_))
        {
            xmlpp::DomParser parser;
            parser.parse_file(filePath_);
            xmlpp::Node* root = parser.get_document()->get_root_node();
            xmlpp::Node* userSettings = root->get_first_child("userSettings");
            xmlpp::Node* properties = userSettings->get_first_child("QMK_Toolbox.Properties.Settings");

            xmlpp::Node::NodeList settingsList = properties->get_children("setting");
            for(xmlpp::Node* settingNode : settingsList)
            {
                xmlpp::Element* settingElement = dynamic_cast<xmlpp::Element*>(settingNode);
                std::string name = settingElement->get_attribute("name")->get_value();
                xmlpp::Node* valueNode = settingNode->get_first_child("value");

                if(name == "hexFileSetting")
                {
                    xmlpp::TextNode* filePathNode = dynamic_cast<xmlpp::TextNode*>(valueNode->get_first_child("text"));
                    // Store active local file to prepend it last
                    activeLocalFile = filePathNode->get_content();
                }
                else if(name == "hexFileCollection")
                {
                    xmlpp::Node::NodeList collectionNodes = valueNode->get_first_child("ArrayOfAnyType")->get_children("anyType");
                    for(xmlpp::Node* collectionNode : collectionNodes)
                    {
                        xmlpp::TextNode* filePathNode = dynamic_cast<xmlpp::TextNode*>(collectionNode->get_first_child("text"));
                        std::string filePath = filePathNode->get_content();
                        localFileBox_->AppendEntry(filePath);
                    }
                }
            }
        }

        if(activeLocalFile != "") localFileBox_->AddActiveEntry(activeLocalFile);
    }

    UserConfiguration::~UserConfiguration()
    {
        // Write settings to user configuration file on exit
        xmlpp::Document outputConfigDocument;
        xmlpp::Element* rootNode = outputConfigDocument.create_root_node("configuration");

        xmlpp::Element* userSettings = rootNode->add_child("userSettings");
        xmlpp::Element* properties = userSettings->add_child("QMK_Toolbox.Properties.Settings");

        // Get local files list
        std::vector<std::string> hexFileList = localFileBox_->GetLocalFilesList();

        // Write local files list
        if(!hexFileList.empty())
        {
            // First write the last loaded selection as hexFileSetting
            xmlpp::Element* settingElement = properties->add_child("setting");
            settingElement->set_attribute("name", "hexFileSetting");
            settingElement->set_attribute("serializeAs", "String");
            xmlpp::Element* valueElement = settingElement->add_child("value");
            std::vector<std::string>::const_iterator hexFileIt = hexFileList.begin();
            valueElement->add_child_text(*hexFileIt);

            // Write the rest of the list under hexFileCollection
            hexFileIt++;
            if(hexFileIt != hexFileList.end())
            {
                settingElement = properties->add_child("setting");
                settingElement->set_attribute("name", "hexFileCollection");
                settingElement->set_attribute("serializeAs", "Xml");
                valueElement = settingElement->add_child("value");
                xmlpp::Element* arrayElement = valueElement->add_child("ArrayOfAnyType");
                arrayElement->set_namespace_declaration("http://www.w3.org/2001/XMLSchema-instance", "xsi");
                arrayElement->set_namespace_declaration("http://www.w3.org/2001/XMLSchema", "xsd");
                for(; hexFileIt != hexFileList.end(); hexFileIt++)
                {
                    xmlpp::Element* arrayEntryElement = arrayElement->add_child("anyType");
                    arrayEntryElement->set_attribute("type", "xsd:string", "xsi");
                    arrayEntryElement->add_child_text(*hexFileIt);
                }
            }
        }

        outputConfigDocument.write_to_file(filePath_, "utf-8");
    }

} // namespace qmk