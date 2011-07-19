#include <map>
#include <tchar.h>
#include <shared/UnicodeSupport.h>
#include <Parsers/AOFullCharacterSync.h>
#include <boost/test/unit_test.hpp>
#include <iostream>
#include <boost/filesystem.hpp>

namespace bfs = boost::filesystem;

struct AOFullCharacterMessageTestFixture
{
    AOFullCharacterMessageTestFixture() { }

    std::vector<char> LoadBinaryData(const char* filename)
    {
        bfs::path path(filename);
        if (!bfs::exists(path)) {
            throw new std::exception("file not found!");
        }

        std::vector<char> result;

        std::ifstream ifs;
        ifs.open(filename, std::ios::in | std::ios::binary | std::ios::ate);

        if (ifs.is_open())
        {
            int size = ifs.tellg();
            ifs.seekg(0);

            char* buffer = new char[size];
            ifs.read(buffer, size);
            result.insert(result.end(), buffer, buffer + size);
            delete buffer;
        }

        return result;
    }
};

BOOST_FIXTURE_TEST_SUITE(AOFullCharacterMessageTests, AOFullCharacterMessageTestFixture);

BOOST_AUTO_TEST_CASE(XplorerFullyBuffedTest)
{
    std::vector<char> data = LoadBinaryData("fullsync_xplorer_entering_shop_buffed.bin");
    BOOST_REQUIRE(data.size() > 0);

    Parsers::AOFullCharacterMessage parser(&*data.begin(), data.size());
}

BOOST_AUTO_TEST_CASE(XplorerZoningWithPerkLockTimerTest)
{
    std::vector<char> data = LoadBinaryData("fullsync_xplorer_perk_timers.bin");
    BOOST_REQUIRE(data.size() > 0);

    Parsers::AOFullCharacterMessage parser(&*data.begin(), data.size());
}

BOOST_AUTO_TEST_SUITE_END();
