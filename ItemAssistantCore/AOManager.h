#ifndef AOMANAGER_H
#define AOMANAGER_H

#include <ItemAssistantCore/ItemAssistantCore.h>
#include <Shared/UnicodeSupport.h>
#include <Shared/Singleton.h>
#include <exception>
#include <vector>


class ITEMASSISTANTCORE_API AOManager
{
    SINGLETON(AOManager);
public:
    ~AOManager();

    struct ITEMASSISTANTCORE_API AOManagerException : public std::exception {
        AOManagerException(std::tstring const& message) : std::exception(to_ascii_copy(message).c_str()) {}
    };

    std::tstring getAOFolder() const;
    bool createAOItemsDB(std::tstring const& localfile, bool showProgress = true);
    std::tstring getCustomBackpackName(unsigned int charid, unsigned int containerid) const;
    std::vector<std::tstring> getAccountNames() const;

    struct ITEMASSISTANTCORE_API DimensionInfo
    {
        std::vector<unsigned int> server_ip;
        unsigned int server_port;
        std::string name;
        std::string description;
    };

    std::vector<DimensionInfo> getDimensions() const;

private:
    mutable std::tstring m_aofolder;
};

#endif // AOMANAGER_H
