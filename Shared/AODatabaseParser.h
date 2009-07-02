#ifndef AODatabaseParser_h__
#define AODatabaseParser_h__

#include <boost/smart_ptr.hpp>
#include <fstream>
#include <shared/AODB.h>
#include <string>


class AODatabaseParser
{
public:
    struct Exception : public std::exception {
        Exception(std::string const& what) : std::exception(what.c_str()) {}
    };

    /// Creates a new AO Database parser for the specified set of files.
    AODatabaseParser(std::vector<std::string> const& aodbfiles);
    ~AODatabaseParser();

    /// Retrieves the item at the specified offset.
    boost::shared_ptr<ao_item> GetItem(unsigned int offset) const;

protected:
    void EnsureFileOpen(unsigned int offset) const;
    std::pair<unsigned int, std::string> GetFileFromOffset(unsigned int offset) const;
    void OpenFileFromOffset(unsigned int offset) const;

private:
    std::map<unsigned int, std::string> m_file_offsets;
    mutable unsigned int m_current_file_offset;
    mutable unsigned int m_current_file_size;
    mutable std::ifstream m_file;
    boost::shared_array<char> m_buffer;
};

#endif // AODatabaseParser_h__
