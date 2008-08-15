#ifndef AODatabaseParser_h__
#define AODatabaseParser_h__

#include <boost/smart_ptr.hpp>
#include <shared/AODB.h>
#include <string>

// Forwards
struct ifil;
struct iidx;
struct iseg;


class AODatabaseParser
{
public:
    AODatabaseParser(std::string const& aodbfile);
    ~AODatabaseParser();

    /// Retrieves the first item of a specific type
    boost::shared_ptr<ao_item> GetFirstItem(ResourceType type);

    /// Retrieves the next item of the current type
    boost::shared_ptr<ao_item> GetNextItem();

private:
    boost::shared_ptr<ifil> m_aodbFile;
    boost::shared_ptr<iidx> m_aodbIndex;
    boost::shared_array<iseg> m_aodbSegment;

    ResourceType m_currentResourceType;
    bool m_isOk;
    boost::shared_array<char> m_buffer;};


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
