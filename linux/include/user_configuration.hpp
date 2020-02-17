#pragma once

#include <string>
#include <vector>

namespace qmk
{

    class UserConfiguration
    {
    public:
        UserConfiguration(const std::string& configDir);
        ~UserConfiguration();
        
        const std::vector<std::string>& GetLocalFileList() const;
        void SetLocalFileList(std::vector<std::string>&& localFileList);

    private:
        std::string filePath_;

        std::vector<std::string> hexFileCollection_;
    };

} // namespace qmk