#pragma once

#include <string>
#include <vector>

namespace qmk
{

    class LocalFileBox;
    class McuListComboBox;

    class UserConfiguration
    {
    public:
        UserConfiguration(const std::string& configDir, LocalFileBox* localFileBox, McuListComboBox* mcuListComboBox);
        ~UserConfiguration();

    private:
        LocalFileBox* localFileBox_;
        McuListComboBox* mcuListComboBox_;
        std::string filePath_;
    };

} // namespace qmk