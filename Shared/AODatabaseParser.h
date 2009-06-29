#ifndef AODatabaseParser_h__
#define AODatabaseParser_h__

#include <boost/smart_ptr.hpp>
#include <shared/AODB.h>
#include <string>
#include <boost/iostreams/device/mapped_file.hpp>


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
    boost::shared_ptr<ao_item> ExtractItem(const char* pos) const;
    void EnsureFileOpen(unsigned int offset) const;

private:
    std::map<unsigned int, std::string> m_file_offsets;
    mutable boost::iostreams::mapped_file_source m_file;
    mutable unsigned int m_current_file_offset;

    //boost::iostreams::mapped_file_source m_file_001;

    // This maps a resource type to a map from resource ID to resource offset in the database file.
    //std::map<ResourceType, std::map<unsigned int, unsigned int> > m_record_index;
    std::map<ResourceType, std::map<unsigned int, unsigned int> >::iterator m_current_resource;
    std::map<unsigned int, unsigned int>::iterator m_current_record;
};


class AOItemParser
    : public ao_item
{
public:
    AOItemParser(char* pBuffer, unsigned int bufSize);

private:
    char* ParseFunctions(char* pBuffer, unsigned int bufSize, unsigned int ftype, unsigned int fkey);
    char* ParseRequirements(char *pBuffer, unsigned int bufSize, std::list<ao_item_req> &reqlist, unsigned int cnt, int rhook);
    char* ParseString(char *pBuffer, unsigned int bufSize, std::string &outText);
};


#endif // AODatabaseParser_h__
