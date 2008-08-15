#include "stdafx.h"
#include "AODatabaseWriter.h"
#include <boost/assign.hpp>
#include <boost/algorithm/string.hpp>

using namespace boost::algorithm;
using namespace boost::assign;


#define SQL_CREATE_ITEMS_TABLE \
    "CREATE TABLE [tblAO] (" \
    "   [aoid] INTEGER  PRIMARY KEY NOT NULL," \
    "   [name] TEXT  NOT NULL," \
    "   [ql] INTEGER  NULL," \
    "   [type] TEXT  NOT NULL," \
    "   [description] VARCHAR(256)  NULL," \
    "   [flags] INTEGER  NULL," \
    "   [properties] INTEGER  NULL," \
    "   [icon] INTEGER  NULL" \
    ");"

#define SQL_CREATE_ATTRIBUTES_TABLE \
    "CREATE TABLE [tbl_Attributes] (" \
    "   [ItemID] INTEGER  NOT NULL," \
    "   [AttributeName] VARCHAR(128)  NOT NULL," \
    "   [AttributeValue] TEXT  NOT NULL," \
    "   PRIMARY KEY ([ItemID],[AttributeName])" \
    ");"


AODatabaseWriter::AODatabaseWriter(std::string const& filename)
{
    m_db.Init(from_ascii_copy(filename));
    m_db.Exec(from_ascii_copy(SQL_CREATE_ITEMS_TABLE));
    //m_db.Exec(from_ascii_copy(SQL_CREATE_ATTRIBUTES_TABLE));
}


AODatabaseWriter::~AODatabaseWriter()
{
    m_db.Term();
}


void AODatabaseWriter::BeginWrite()
{
    m_db.Begin();
}


void AODatabaseWriter::WriteItem(boost::shared_ptr<ao_item> item)
{
    static std::map<unsigned int, std::string> s_ItemTypeMap = boost::assign::map_list_of
        (AODB_ITEM_MISC,    "Misc")
        (AODB_ITEM_WEAPON,  "Weapon")
        (AODB_ITEM_ARMOR,   "Armor")
        (AODB_ITEM_IMPLANT, "Implant")
        (AODB_ITEM_TEMPLATE,"Template")
        (AODB_ITEM_SPIRIT,  "Spirit");

    if (item->name.empty() || item->ql == 0) {
        return;
    }

    if (s_ItemTypeMap.find(item->type) == s_ItemTypeMap.end()) {
        assert(false);  // Found unknown object type!
        return;
    }

    // We need to escape the ' symbol inside the strings to have the SQL be valid.
    std::string name = boost::algorithm::replace_all_copy(item->name, "'", "''");
    std::string desc = boost::algorithm::replace_all_copy(item->description, "'", "''");

    std::ostringstream sql;
    sql << "INSERT INTO tblAO (aoid, name, ql, type, description, flags, properties, icon) VALUES (" 
        << item->aoid << ", " 
        << "'" << name << "', " 
        << item->ql << ", " 
        << "'" << s_ItemTypeMap[item->type] << "', " 
        << "'" << desc << "', "
        << item->flags << ", "
        << item->props << ", "
        << item->icon << ")";

    m_db.Exec(from_ascii_copy(sql.str()));
}


void AODatabaseWriter::CommitItems()
{
    m_db.Commit();
}


void AODatabaseWriter::AbortWrite()
{
    m_db.Rollback();
}
