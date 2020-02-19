#pragma once

#include <string>
#include <vector>

namespace qmk
{

    class LocalFileBox;

    class UserConfiguration
    {
    public:
        UserConfiguration(const std::string& configDir, LocalFileBox* localFileBox);
        ~UserConfiguration();

    private:
        LocalFileBox* localFileBox_;
        std::string filePath_;
    };

} // namespace qmk