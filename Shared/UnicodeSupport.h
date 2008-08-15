#pragma once

#include <string>
#include <sstream>

namespace std {
   typedef basic_string<TCHAR> tstring;
   typedef basic_stringstream<TCHAR> tstringstream;
}


std::string to_ascii_copy(std::tstring const& input);
std::string to_utf8_copy(std::tstring const& input);
std::tstring from_ascii_copy(std::string const& input);
std::tstring from_utf8_copy(std::string const& input);


#ifdef UNICODE
#define STREAM2STR( streamdef ) \
    (((std::wostringstream&)(std::wostringstream().flush() << streamdef)).str())
#else
#define STREAM2STR( streamdef ) \
    (((std::ostringstream&)(std::ostringstream().flush() << streamdef)).str())
#endif
