#pragma once

#include <string>
#include <sstream>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>


namespace std {
   typedef basic_string<TCHAR> tstring;
   typedef basic_stringstream<TCHAR> tstringstream;
   typedef basic_ostringstream<TCHAR> tostringstream;
}


/// Typedef a character independant path class as tpath.
namespace boost { namespace filesystem {
#ifdef UNICODE
    typedef wpath tpath;
#else
    typedef path tpath;
#endif

}}  // namespace boost::filesystem


/// Typedef a character independant format class as tformat.
namespace boost {
    typedef basic_format<TCHAR> tformat;
}


std::string to_ascii_copy(std::wstring const& input);
std::string to_ascii_copy(std::string const& input);
std::string to_utf8_copy(std::tstring const& input);
std::tstring from_ascii_copy(std::string const& input);
std::tstring from_utf8_copy(std::string const& input);

#define STREAM2STR( streamdef ) \
    (((std::tostringstream&)(std::tostringstream().flush() << streamdef)).str())
