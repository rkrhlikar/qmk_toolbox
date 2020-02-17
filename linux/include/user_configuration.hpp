#pragma once

#include <string>
#include <vector>

namespace qmk
{

    class UserConfiguration
    {
    public:
        UserConfiguration();
        UserConfiguration(const std::string& configDir);
        ~UserConfiguration();
        
        const std::vector<std::string>& GetLocalFileList() const;

    private:
        std::string filePath_;

        std::vector<std::string> hexFileCollection_;
    };

} // namespace qmk