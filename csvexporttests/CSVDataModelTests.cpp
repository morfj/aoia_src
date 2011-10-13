#include <gtest/gtest.h>
#include <mockitopp/mockitopp.hpp>
#include <csvexport/CSVDataModel.h>

using namespace aoia;
using namespace mockitopp::matcher;

namespace
{
    struct null_deleter
    {
        void operator()(void const*) const
        {
        }
    };
}


TEST(CSVDataModelTests, Construction_Always_RunQueryOnce)
{
    // Arrange
    mockitopp::mock_object<IDBManager> mockDbManager;
    mockitopp::mock_object<IContainerManager> mockContainerManager;

    boost::shared_ptr<IDBManager> dbmanager(&mockDbManager.getInstance(), null_deleter());
    boost::shared_ptr<IContainerManager> containermanager(&mockContainerManager.getInstance(), null_deleter());

    SQLite::TablePtr tbl(new SQLite::Table(0, 0, NULL));
    mockDbManager(static_cast<SQLite::TablePtr (IDBManager::*)(std::tstring const&) const>(&IDBManager::ExecTable))
        .when(any<std::tstring const&>())
        .thenReturn(tbl);

    // Act
    CSVDataModel subject(dbmanager, containermanager, _T(""), _T(""));

    // Assert
    EXPECT_TRUE(mockDbManager(static_cast<SQLite::TablePtr (IDBManager::*)(std::tstring const&) const>(&IDBManager::ExecTable)).exactly(1));
}
