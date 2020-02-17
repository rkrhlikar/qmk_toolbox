#include "user_configuration.hpp"

#include <experimental/filesystem>

#include <libxml++/libxml++.h>

namespace qmk
{

    UserConfiguration::UserConfiguration() {}

    UserConfiguration::UserConfiguration(const std::string& configDir)
    {
        namespace fs = std::experimental::filesystem;

        filePath_ = configDir + "/user.config";

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
                    std::string filePath = filePathNode->get_content();
                    hexFileCollection_.insert(hexFileCollection_.begin(), filePath);
                }
                else if(name == "hexFileCollection")
                {
                    xmlpp::Node::NodeList collectionNodes = valueNode->get_first_child("ArrayOfAnyType")->get_children("anyType");
                    for(xmlpp::Node* collectionNode : collectionNodes)
                    {
                        xmlpp::TextNode* filePathNode = dynamic_cast<xmlpp::TextNode*>(collectionNode->get_first_child("text"));
                        std::string filePath = filePathNode->get_content();
                        hexFileCollection_.push_back(filePath);
                    }
                }
            }
        }
    }

    UserConfiguration::~UserConfiguration() {}

    const std::vector<std::string>& UserConfiguration::GetLocalFileList() const
    {
        return hexFileCollection_;
    }

} // namespace qmk