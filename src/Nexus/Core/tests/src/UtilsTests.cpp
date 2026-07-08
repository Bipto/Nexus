#include <gtest/gtest.h>

#include "Core/Utils.hpp"

TEST(BitMacroTest, ProducesCorrectBitmask)
{
    EXPECT_EQ(BIT(0), 1);
    EXPECT_EQ(BIT(1), 2);
    EXPECT_EQ(BIT(2), 4);
    EXPECT_EQ(BIT(5), 32);
}

#define VALUE 123

TEST(StringifyMacroTest, StringifyDoesNotExpand)
{
    // STRINGIFY(x) → "x"
    EXPECT_STREQ(STRINGIFY(VALUE), "VALUE");
}

TEST(StringifyMacroTest, ToStringExpandsThenStringifies)
{
    // TOSTRING(x) → expands VALUE → "123"
    EXPECT_STREQ(TOSTRING(VALUE), "123");
}