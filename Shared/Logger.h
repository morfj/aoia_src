#pragma once

#include <string>
#include <fstream>
#include <Shared/UnicodeSupport.h>


class Logger
{
public:
    static Logger* instance();
    ~Logger(void);

    void init(std::tstring const& filename, std::tstring const& versioninfo);
    void destroy();
    void log(std::tstring const& msg);

protected:
    Logger();

private:
    std::ofstream m_out;
    bool m_enabled;
};
