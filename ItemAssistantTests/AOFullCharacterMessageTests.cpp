#include <map>
#include <tchar.h>
#include <shared/UnicodeSupport.h>
#include <ItemAssistantCore/Logger.h>
#include "ItemAssistant/AOMessageParsers.h"
#include <boost/test/unit_test.hpp>
#include "AOFullCharacterMessageTestsData.h"


struct AOFullCharacterMessageTestFixture
{
    AOFullCharacterMessageTestFixture()
    {
        LoadTestData(testdata1, m_testData1);
        LoadTestData(testdata2, m_testData2);
    }

    void LoadTestData(const char* data, std::vector<unsigned int> &output)
    {
        std::istringstream is(data);
        while (is.good())
        {
            unsigned int val;
            is >> std::hex >> val;
            output.push_back(_byteswap_ulong(val));
        }
    }

    std::vector<unsigned int> m_testData1;
    std::vector<unsigned int> m_testData2;
};

BOOST_FIXTURE_TEST_SUITE(AOFullCharacterMessageTests, AOFullCharacterMessageTestFixture);

BOOST_AUTO_TEST_CASE(ParseTest1)
{
    Parsers::AOFullCharacterMessage parser((char*)&*(m_testData1.begin()), m_testData1.size() * sizeof(unsigned int));

    std::vector<Parsers::AOContainerItemPtr> items;
    parser.getAllItems(items);
    BOOST_CHECK_EQUAL(items.size(), parser.numitems());
}

BOOST_AUTO_TEST_CASE(ParseTest2)
{
    Parsers::AOFullCharacterMessage parser((char*)&*(m_testData2.begin()), m_testData2.size() * sizeof(unsigned int));

    std::vector<Parsers::AOContainerItemPtr> items;
    parser.getAllItems(items);
    BOOST_CHECK_EQUAL(items.size(), parser.numitems());
}

BOOST_AUTO_TEST_SUITE_END();
