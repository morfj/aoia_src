#ifndef AODatabaseWriter_h__
#define AODatabaseWriter_h__

#include <boost/smart_ptr.hpp>
#include <shared/SQLite.h>
#include <string>
#include <Shared/AODB.h>


#define CURRENT_AODB_VERSION 4


class AODatabaseWriter
{
public:
    AODatabaseWriter(std::string const& filename);
    ~AODatabaseWriter();

    void BeginWrite();
    void WriteItem(boost::shared_ptr<ao_item> item);
    void CommitItems();
    void AbortWrite();
    void PostProcessData();

protected:
    void writeRequirement(unsigned int aoid, ao_item_req const& req);
    void writeEffect(unsigned int aoid, ao_item_effect const& eff);

private:
    SQLite::Db m_db;
};

#endif // AODatabaseWriter_h__
