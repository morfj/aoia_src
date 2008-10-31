#include "StdAfx.h"
#include "Logger.h"

boost::shared_ptr<Logger> g_logger;

#define LB "\n\r"

Logger::Logger()
  : m_enabled(false)
{
}


Logger::~Logger(void)
{
}


Logger* Logger::instance()
{
    if (!g_logger) {
        g_logger.reset(new Logger());
    }
    return g_logger.get();
}


void Logger::init(std::tstring const& filename, std::tstring const& versioninfo)
{
    m_enabled = !filename.empty();
    if (m_enabled) {
        m_out.open(filename.c_str());
        log(STREAM2STR(
                 "***********************"  << LB << std::flush
              << "    Logging Started"      << LB << std::flush
              << "***********************"  << LB << std::flush
              << "Version: " << versioninfo << LB << std::flush
              << "***********************"  << LB << std::flush
        ));
    }
}


void Logger::destroy()
{
    log(STREAM2STR(
             "***********************" << LB << std::flush
          << "   Logging Terminated  " << LB << std::flush
          << "***********************" << LB << std::flush
    ));
    if (m_out.is_open()) {
        m_out.close();
    }
    m_enabled = false;
}


void Logger::log(std::tstring const& msg)
{
    if (m_enabled && m_out.good()) {
        m_out << to_ascii_copy(msg) << LB << std::flush;
    }
}
