#ifndef LOGGER_H
#define LOGGER_H

#include <ItemAssistantCore/ItemAssistantCore.h>
#include <string>
#include <fstream>
#include <Shared/UnicodeSupport.h>
#include <Shared/Singleton.h>


class ITEMASSISTANTCORE_API Logger
{
    SINGLETON(Logger);
public:
    ~Logger();

    void init(std::tstring const& filename, std::tstring const& versioninfo);
    void destroy();
    void log(std::tstring const& msg);

private:
    std::ofstream m_out;
    bool m_enabled;
};

#endif // LOGGER_H
