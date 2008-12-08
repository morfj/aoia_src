#ifndef AOMANAGER_H
#define AOMANAGER_H

#include <ItemAssistantCore/ItemAssistantCore.h>
#include <Shared/UnicodeSupport.h>
#include <Shared/Singleton.h>
#include <vector>
#include <exception>


class ITEMASSISTANTCORE_API AOManager
{
    SINGLETON(AOManager);
public:
    ~AOManager();

    struct AOManagerException : public std::exception {
        AOManagerException(std::tstring const& message) : std::exception(to_ascii_copy(message).c_str()) {}
    };

    std::tstring getAOFolder() const;
    bool createAOItemsDB(std::tstring const& localfile, bool showProgress = true);
    std::tstring getCustomBackpackName(unsigned int charid, unsigned int containerid) const;
    std::vector<std::tstring> getAccountNames() const;

private:
    mutable std::tstring m_aofolder;
};

#endif // AOMANAGER_H
