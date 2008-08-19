#include "stdafx.h"
#include "AODatabaseWriter.h"
#include <boost/assign.hpp>
#include <boost/algorithm/string.hpp>
#include "Logger.h"


using namespace boost::algorithm;
using namespace boost::assign;

const std::string c_scheme_sql = 
    "CREATE TABLE tblAO ("
    "   [aoid] INTEGER  PRIMARY KEY NOT NULL,"
    "   [name] TEXT  NOT NULL,"
    "   [ql] INTEGER  NULL,"
    "   [type] TEXT  NOT NULL,"
    "   [description] VARCHAR(256)  NULL,"
    "   [flags] INTEGER  NULL,"
    "   [properties] INTEGER  NULL,"
    "   [icon] INTEGER  NULL"
    "   );"

    "CREATE TABLE [tblPocketBoss] ("
    "   [pbid] INTEGER  NOT NULL PRIMARY KEY,"
    "   [ql] INTEGER  NOT NULL,"
    "   [name] TEXT  UNIQUE NOT NULL"
    "   );"

    "CREATE TABLE tblPatterns ( "
    "    [aoid] INTEGER NOT NULL PRIMARY KEY UNIQUE, "
    "    [pattern] TEXT NOT NULL,"
    "    [name] TEXT NOT NULL"
    "    );"

    "CREATE VIEW vSchemeVersion AS SELECT '1' AS Version ; "

    ;


const std::vector<std::string> c_datatransformation_sql = list_of
    ("INSERT OR REPLACE INTO tblPocketBoss (pbid, ql, name) SELECT aoid, ql, substr(name, 34, length(name)-34) AS name FROM tblAO WHERE name LIKE '%Novictalized Notum Crystal with%' AND LENGTH(name) > 35 ORDER BY aoid")
    ("INSERT OR REPLACE INTO tblPatterns (aoid, name, pattern) SELECT aoid, TRIM(REPLACE(REPLACE(REPLACE(REPLACE(REPLACE(name, 'Pattern', ''), 'of', ''), 'Aban', ''), 'Abhan', ''), '''', '')) AS name, 'A' AS Pattern FROM tblAO WHERE name LIKE 'ab%an pattern%'")
    ("INSERT OR REPLACE INTO tblPatterns (aoid, name, pattern) SELECT aoid, TRIM(REPLACE(REPLACE(REPLACE(REPLACE(name, 'Pattern', ''), 'of', ''), 'Bhotaar', ''), '''', '')) AS name, 'B' AS Pattern FROM tblAO WHERE name LIKE 'b%ar pattern%'")
    ("INSERT OR REPLACE INTO tblPatterns (aoid, name, pattern) SELECT aoid, TRIM(REPLACE(REPLACE(REPLACE(REPLACE(name, 'Pattern', ''), 'of', ''), 'Chi', ''), '''', '')) AS name, 'C' AS Pattern FROM tblAO WHERE name LIKE 'chi pattern%'")
    ("INSERT OR REPLACE INTO tblPatterns (aoid, name, pattern) SELECT aoid, TRIM(REPLACE(REPLACE(REPLACE(REPLACE(name, 'Pattern', ''), 'of', ''), 'Dom', ''), '''', '')) AS name, 'D' AS Pattern FROM tblAO WHERE name LIKE 'dom pattern%'")
    ("INSERT OR REPLACE INTO tblPatterns (aoid, name, pattern) SELECT aoid, TRIM(REPLACE(REPLACE(REPLACE(REPLACE(name, 'Assembly', ''), 'of', ''), 'Aban-Bhotar', ''), '''', '')) AS name, 'AB' AS Pattern FROM tblAO WHERE name LIKE 'a%-b%ar assembly%'")
    ("INSERT OR REPLACE INTO tblPatterns (aoid, name, pattern) SELECT aoid, TRIM(REPLACE(REPLACE(name, 'Aban-Bhotar-Chi Assembly', ''), '''', '')) AS name, 'ABC' AS Pattern FROM tblAO WHERE name LIKE 'a%b%c% assembly%'")
    ("INSERT OR REPLACE INTO tblPatterns (aoid, name, pattern) SELECT aoid, TRIM(REPLACE(REPLACE(REPLACE(name, 'Complete Blueptrint Pattern of', ''), 'Complete Blueprint Pattern of', ''), '''', '')) AS name, 'ABCD' AS Pattern FROM tblAO WHERE name LIKE '%complete%pattern of ''%'")
    ;


AODatabaseWriter::AODatabaseWriter(std::string const& filename)
{
    m_db.Init(from_ascii_copy(filename));
    m_db.Exec(from_ascii_copy(c_scheme_sql));
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


void AODatabaseWriter::PostProcessData()
{
    for (std::vector<std::string>::const_iterator it = c_datatransformation_sql.begin(); it != c_datatransformation_sql.end(); ++it) {
        if (!m_db.Exec(*it)) {
            assert(false);
            Logger::instance()->log(_T("Error while postprocessing data."));
        }
    }
}
