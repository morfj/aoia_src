#ifndef AODatabaseParser_h__
#define AODatabaseParser_h__

#include <boost/smart_ptr.hpp>
#include <shared/AODB.h>
#include <string>
#include <boost/iostreams/device/mapped_file.hpp>


class AODatabaseParser
{
public:
    struct CTreeDbException : public std::exception {
        CTreeDbException(std::string const& what) : std::exception(what.c_str()) {}
    };

    AODatabaseParser(std::string const& aodbfile);
    ~AODatabaseParser();

    /// Retrieves the number of items with the specified type
    unsigned int GetItemCount(ResourceType type);

    /// Retrieves the first item of a specific type
    boost::shared_ptr<ao_item> GetFirstItem(ResourceType type);

    /// Retrieves the next item of the current type
    boost::shared_ptr<ao_item> GetNextItem();

protected:
    void ReadIndexFile(std::string filename);
    const char* ReadIndexBlock(const char* pos, const char* end);
    boost::shared_ptr<ao_item> ExtractItem(const char* pos);

private:
    bool m_isOk;
    boost::iostreams::mapped_file_source m_file;
    //boost::iostreams::mapped_file_source m_file_001;

    // This maps a resource type to a map from resource ID to resource offset in the database file.
    std::map<ResourceType, std::map<unsigned int, unsigned int> > m_record_index;
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
