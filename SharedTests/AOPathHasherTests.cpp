#include <gtest/gtest.h>
#include <shared/AOPathHasher.h>
#include <boost/filesystem/path.hpp>

using namespace boost;


TEST(AOPathHasherTests, HashParentPath_C_ReturnsExpectedHash)
{
    EXPECT_EQ(0xcd94ae8e, shared::HashParentPath(boost::filesystem::path("C:\\somefolder")));
}

TEST(AOPathHasherTests, HashParentPath_D_ReturnsExpectedHash)
{
    EXPECT_EQ(0xcd94ae4f, shared::HashParentPath(boost::filesystem::path("D:\\somefolder")));
}

TEST(AOPathHasherTests, HashParentPath_CFuncom_ReturnsExpectedHash)
{
    EXPECT_EQ(0x70dad3e6, shared::HashParentPath(boost::filesystem::path("C:\\Funcom\\somefolder")));
}

TEST(AOPathHasherTests, HashParentPath_DSomeDeeplyNestedFoldersWithStrangeSyntaxesAnarchyOnlineLive_ReturnsExpectedHash)
{
    EXPECT_EQ(0x3e6099b5, shared::HashParentPath(boost::filesystem::path("D:\\some\\deeply\\nested\\Folders with\\STRAAAnge\\--syntaxes--\\Anarchy Online Live")));
}

TEST(AOPathHasherTests, HashParentPath_WithTrailingSlash_ReturnsExpectedHash)
{
    EXPECT_EQ(0x70dad3e6, shared::HashParentPath(boost::filesystem::path("C:\\Funcom\\somefolder\\")));
}
